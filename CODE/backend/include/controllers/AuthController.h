// ====== ./include/controllers/AuthController.h ======
/**
 * @file AuthController.h
 * @brief Contrôleur d'authentification utilisant le pattern HttpSimpleController de Drogon
 * @author Meta League Backend Team
 * @date 2025
 * @version 1.0
 *
 * RESPONSABILITÉS :
 * - Gérer toutes les routes d'authentification (/api/auth/)
 * - Valider les requêtes HTTP entrantes
 * - Orchestrer les services d'authentification
 * - Retourner des réponses HTTP formatées selon les standards API
 *
 * ARCHITECTURE :
 * - Hérite de HttpSimpleController<AuthController> (pattern Drogon)
 * - Utilise PATH_LIST pour déclarer les routes automatiquement
 * - Point d'entrée unique via asyncHandleHttpRequest()
 * - Délègue la logique métier au AuthService
 *
 * ROUTES GÉRÉES :
 * - POST /api/auth/register : Inscription utilisateur
 * - POST /api/auth/login    : Connexion utilisateur  
 * - GET  /api/auth/me       : Profil utilisateur courant (protégé JWT)
 *
 * DÉPENDANCES :
 * - services/AuthService : Logique métier d'authentification
 * - dto/auth/ : Contrats d'API pour les requêtes/réponses
 * - middlewares/JwtMiddleware : Protection JWT pour /me
 *
 * UTILISATION :
 * ```cpp
 * // Contrôleur auto-détecté par Drogon, pas d'instanciation manuelle
 * // Les routes sont automatiquement enregistrées via PATH_LIST
 * 
 * // Exemple d'appel client :
 * curl -X POST http://localhost:8080/api/auth/login \
 *      -H "Content-Type: application/json" \
 *      -d '{"username":"bob","password":"secret"}'
 * ```
 *
 * BONNES PRATIQUES :
 * - Thread-safe (stateless, pas de variables globales)
 * - Validation systématique des entrées via DTOs
 * - Gestion d'erreurs homogène avec codes HTTP appropriés
 * - Logging détaillé pour debugging et sécurité
 * - Séparation claire : contrôleur = orchestration, service = logique métier
 */

#pragma once
#include <drogon/HttpSimpleController.h>
#include "services/AuthService.h"
#include <memory>

using namespace drogon;

namespace controllers {

/**
 * @class AuthController
 * @brief Contrôleur principal pour toutes les opérations d'authentification
 *
 * Cette classe implémente le pattern HttpSimpleController de Drogon qui permet :
 * - Déclaration des routes via PATH_LIST (auto-découverte par Drogon)
 * - Point d'entrée unique asyncHandleHttpRequest() pour toutes les routes
 * - Gestion async native compatible avec l'architecture Drogon
 *
 * PATTERN DE FONCTIONNEMENT :
 * 1. Drogon appelle automatiquement asyncHandleHttpRequest()
 * 2. Le contrôleur route vers le bon handler selon path + method
 * 3. Chaque handler valide, appelle le service, et retourne la réponse
 *
 * EXEMPLE D'EXTENSION :
 * ```cpp
 * // Pour ajouter POST /api/auth/forgot-password :
 * 
 * // 1. Dans PATH_LIST :
 * PATH_ADD("/api/auth/forgot-password", Post);
 * 
 * // 2. Dans asyncHandleHttpRequest() :
 * else if (path == "/api/auth/forgot-password") {
 *     handleForgotPassword(req, std::move(callback));
 * }
 * 
 * // 3. Implémenter handleForgotPassword()
 * ```
 */
class AuthController : public drogon::HttpSimpleController<AuthController> {
private:
    std::shared_ptr<services::AuthService> authService_;

public:
    /**
     * @brief Constructeur du contrôleur d'authentification
     * 
     * Initialise le service d'authentification avec ses dépendances :
     * - Récupère le client DB depuis l'app Drogon
     * - Instancie UserRepository avec le client DB
     * - Instancie AuthService avec le repository
     * 
     * ARCHITECTURE DES DÉPENDANCES :
     * AuthController → AuthService → UserRepository → DbClient
     * 
     * @note Ce constructeur est appelé automatiquement par Drogon
     * @note Thread-safe : chaque instance a ses propres dépendances
     */
    AuthController();

    /**
     * @brief Déclaration des routes HTTP gérées par ce contrôleur
     * 
     * Le macro PATH_LIST de Drogon permet de déclarer toutes les routes
     * que ce contrôleur peut traiter. Drogon les enregistre automatiquement.
     * 
     * FORMAT : PATH_ADD(route, méthodes..., [middleware])
     * 
     * ROUTES ACTUELLES :
     * - /api/auth/register : POST (inscription)
     * - /api/auth/login    : POST (connexion)
     * - /api/auth/me       : GET avec JwtMiddleware (profil utilisateur)
     * 
     * @note Les middlewares sont appliqués dans l'ordre déclaré
     * @note Supporte GET, Post, Put, Delete, Options, etc.
     */
    PATH_LIST_BEGIN
    PATH_ADD("/api/auth/register", Post, "middlewares::RateLimitFilter");
    PATH_ADD("/api/auth/login", Post, "middlewares::RateLimitFilter");
    PATH_ADD("/api/auth/me", Get, "middlewares::JwtMiddleware");
    PATH_LIST_END

    /**
     * @brief Point d'entrée unique pour toutes les requêtes HTTP de ce contrôleur
     * 
     * Cette méthode est appelée automatiquement par Drogon pour chaque requête
     * correspondant aux routes déclarées dans PATH_LIST.
     * 
     * RESPONSABILITÉS :
     * - Router la requête vers le bon handler selon path + method
     * - Gérer les routes non reconnues (404)
     * - Maintenir l'architecture async de Drogon
     * 
     * @param req Requête HTTP reçue (headers, body, params, etc.)
     * @param callback Fonction de callback pour retourner la réponse HTTP
     * 
     * PATTERN ASYNC :
     * ```cpp
     * // JAMAIS de return dans cette fonction !
     * // Toujours appeler le callback avec la réponse
     * void asyncHandleHttpRequest(..., std::function<void(HttpResponsePtr&)>&& callback) {
     *     // Traitement...
     *     auto response = createResponse();
     *     callback(response);  // ← OBLIGATOIRE
     * }
     * ```
     * 
     * @note Thread-safe : chaque requête a son propre contexte
     * @note Async : ne jamais bloquer cette méthode
     */
    void asyncHandleHttpRequest(const HttpRequestPtr& req, 
                              std::function<void(const HttpResponsePtr&)>&& callback) override;

private:
    // ==================== HANDLERS DE ROUTES ====================
    
    /**
     * @brief Gestionnaire pour l'inscription utilisateur (POST /api/auth/register)
     * 
     * PROCESSUS COMPLET :
     * 1. Validation du JSON reçu
     * 2. Parsing vers RegisterRequest DTO
     * 3. Validation métier (format email, longueur password, etc.)
     * 4. Appel du service d'authentification
     * 5. Retour de la réponse (token + données utilisateur si succès)
     * 
     * @param req Requête HTTP POST avec body JSON RegisterRequest
     * @param callback Callback pour retourner la réponse HTTP
     * 
     * EXEMPLE DE REQUÊTE :
     * ```bash
     * curl -X POST http://localhost:8080/api/auth/register \
     *      -H "Content-Type: application/json" \
     *      -d '{
     *        "username": "bob_martin", 
     *        "email": "bob@example.com",
     *        "password": "monMotDePasse123"
     *      }'
     * ```
     * 
     * RÉPONSE SUCCÈS (201) :
     * ```json
     * {
     *   "success": true,
     *   "message": "Registration successful",
     *   "token": "eyJhbGciOiJIUzI1NiIs...",
     *   "user": {
     *     "id": 42,
     *     "username": "bob_martin",
     *     "email": "bob@example.com",
     *     "is_active": true,
     *     "created_at": "2025-01-15 14:30:25"
     *   }
     * }
     * ```
     * 
     * RÉPONSE ERREUR (400) :
     * ```json
     * {
     *   "success": false,
     *   "message": "Registration failed",
     *   "errors": ["Username already exists", "Invalid email format"]
     * }
     * ```
     * 
     * @note Retourne 201 Created si succès, 400 Bad Request si échec
     * @note Le token JWT permet l'authentification immédiate post-inscription
     */
    void handleRegister(const HttpRequestPtr& req, 
                       std::function<void(const HttpResponsePtr&)>&& callback);

    /**
     * @brief Gestionnaire pour la connexion utilisateur (POST /api/auth/login)
     * 
     * PROCESSUS COMPLET :
     * 1. Validation du JSON reçu  
     * 2. Parsing vers LoginRequest DTO
     * 3. Appel du service pour vérifier les credentials
     * 4. Si valide : génération token JWT + données utilisateur
     * 5. Retour de la réponse authentifiée
     * 
     * @param req Requête HTTP POST avec body JSON LoginRequest
     * @param callback Callback pour retourner la réponse HTTP
     * 
     * EXEMPLE DE REQUÊTE :
     * ```bash
     * curl -X POST http://localhost:8080/api/auth/login \
     *      -H "Content-Type: application/json" \
     *      -d '{
     *        "username": "bob_martin",
     *        "password": "monMotDePasse123"
     *      }'
     * ```
     * 
     * RÉPONSE SUCCÈS (200) :
     * ```json
     * {
     *   "success": true,
     *   "message": "Authentication successful", 
     *   "token": "eyJhbGciOiJIUzI1NiIs...",
     *   "user": {
     *     "id": 42,
     *     "username": "bob_martin",
     *     "email": "bob@example.com", 
     *     "is_active": true,
     *     "created_at": "2025-01-15 14:30:25"
     *   }
     * }
     * ```
     * 
     * RÉPONSE ERREUR (401) :
     * ```json
     * {
     *   "success": false,
     *   "message": "Invalid credentials"
     * }
     * ```
     * 
     * @note Retourne 200 OK si succès, 401 Unauthorized si échec
     * @note Ne jamais révéler si c'est le username ou password qui est faux (sécurité)
     */
    void handleLogin(const HttpRequestPtr& req, 
                    std::function<void(const HttpResponsePtr&)>&& callback);

    /**
     * @brief Gestionnaire pour récupérer le profil utilisateur courant (GET /api/auth/me)
     * 
     * Cette route est PROTÉGÉE par JwtMiddleware qui :
     * 1. Vérifie la présence du header Authorization: Bearer <token>
     * 2. Valide et décode le token JWT
     * 3. Injecte les infos utilisateur dans req->attributes()
     * 4. Permet l'accès à cette méthode
     * 
     * PROCESSUS :
     * 1. Récupération des données depuis req->attributes() (injectées par le middleware)
     * 2. Formatage de la réponse avec les informations utilisateur
     * 3. Retour des données (sans infos sensibles comme password_hash)
     * 
     * @param req Requête HTTP GET avec header Authorization valide
     * @param callback Callback pour retourner la réponse HTTP
     * 
     * EXEMPLE DE REQUÊTE :
     * ```bash
     * curl -X GET http://localhost:8080/api/auth/me \
     *      -H "Authorization: Bearer eyJhbGciOiJIUzI1NiIs..."
     * ```
     * 
     * RÉPONSE SUCCÈS (200) :
     * ```json
     * {
     *   "success": true,
     *   "message": "User information retrieved",
     *   "data": {
     *     "id": 42,
     *     "username": "bob_martin", 
     *     "email": "bob@example.com"
     *   }
     * }
     * ```
     * 
     * RÉPONSE ERREUR (401) - gérée par JwtMiddleware :
     * ```json
     * {
     *   "success": false,
     *   "message": "Invalid or expired token"
     * }
     * ```
     * 
     * @note Cette méthode n'est appelée QUE si le JWT est valide
     * @note Les données viennent du token, pas de la base (performance)
     */
    void handleMe(const HttpRequestPtr& req, 
                 std::function<void(const HttpResponsePtr&)>&& callback);

    // ==================== MÉTHODES UTILITAIRES ====================

    /**
     * @brief Crée une réponse HTTP d'erreur standardisée
     * 
     * Utilise le DTO ApiResponse pour créer une réponse d'erreur homogène
     * avec le bon code de statut HTTP.
     * 
     * @param message Message d'erreur à retourner au client
     * @param status Code de statut HTTP (défaut: 400 Bad Request)
     * @return HttpResponsePtr Réponse HTTP formatée prête à envoyer
     * 
     * EXEMPLE D'UTILISATION :
     * ```cpp
     * if (!json) {
     *     callback(createErrorResponse("JSON body required", HttpStatusCode::k400BadRequest));
     *     return;
     * }
     * ```
     * 
     * RÉPONSE GÉNÉRÉE :
     * ```json
     * {
     *   "success": false,
     *   "message": "JSON body required"
     * }
     * ```
     */
    HttpResponsePtr createErrorResponse(const std::string& message, 
                                      HttpStatusCode status = HttpStatusCode::k400BadRequest);

    /**
     * @brief Crée une réponse HTTP d'erreur de validation avec liste des erreurs
     * 
     * Spécialement conçue pour les erreurs de validation de DTOs qui peuvent
     * contenir plusieurs messages d'erreur (email invalide + password trop court, etc.)
     * 
     * @param errors Liste des messages d'erreur de validation
     * @return HttpResponsePtr Réponse HTTP 400 avec détail des erreurs
     * 
     * EXEMPLE D'UTILISATION :
     * ```cpp
     * auto registerReq = RegisterRequest::fromJson(*json);
     * if (!registerReq.isValid()) {
     *     callback(createValidationErrorResponse(registerReq.getErrors()));
     *     return;
     * }
     * ```
     * 
     * RÉPONSE GÉNÉRÉE
     *
     * ```json
     * {
     *   "success": false,
     *   "message": "Validation errors occurred",
     *   "errors": [
     *     "email is invalid",
     *     "password is too short"
     *   ]
     * }
     * ```
     */

    HttpResponsePtr createValidationErrorResponse(const std::vector<std::string>& errors);
};

} // namespace controllers


/*class AuthController : public drogon::HttpSimpleController<AuthController> {
private:
    std::shared_ptr<services::AuthService> authService_;

public:
    AuthController();

    // Drogon simple controller route list
    PATH_LIST_BEGIN
    PATH_ADD("/api/auth/register", Post);
    PATH_ADD("/api/auth/login", Post);
    PATH_ADD("/api/auth/me", Get, "middlewares::JwtMiddleware");
    PATH_LIST_END

    // Main entry point for all routes
    void asyncHandleHttpRequest(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) override;

private:
    // Route helpers
    void handleRegister(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback);
    void handleLogin(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback);
    void handleMe(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback);

    // Helpers
    HttpResponsePtr createErrorResponse(const std::string& message, HttpStatusCode status = HttpStatusCode::k400BadRequest);*/