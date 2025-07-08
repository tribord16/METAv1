// ====== ./include/services/AuthService.h ======
/**
 * @file AuthService.h
 * @brief Service d'authentification - Couche de logique métier
 * @author Meta League Backend Team
 * @date 2025
 * @version 1.0
 *
 * RESPONSABILITÉS :
 * - Orchestrer les opérations d'authentification (registration, login)
 * - Implémenter la logique métier et les règles de validation
 * - Coordonner UserRepository, CryptoUtils et JwtService
 * - Gérer les erreurs métier et les transformer en réponses appropriées
 *
 * ARCHITECTURE :
 * ```
 * Controller → AuthService → UserRepository → Database
 *                     ↓
 *               CryptoUtils (hash/verify passwords)
 *                     ↓  
 *               JwtService (generate tokens)
 * ```
 *
 * PATTERN ASYNC :
 * - Toutes les méthodes utilisent des callbacks (compatible Drogon)
 * - Pas de retour direct, toujours via std::function<void(...)>
 * - Gestion d'erreur par callback séparé ou optional/variant
 *
 * SÉCURITÉ :
 * - Hash BCrypt pour tous les mots de passe (CryptoUtils)
 * - Validation complète avant création utilisateur
 * - Messages d'erreur génériques (pas de révélation d'infos)
 * - Logging détaillé pour audit sécurité
 *
 * EXEMPLE D'UTILISATION :
 * ```cpp
 * auto userRepo = std::make_shared<UserRepository>(dbClient);
 * auto authService = std::make_shared<AuthService>(userRepo);
 * 
 * // Inscription
 * RegisterRequest req{"bob", "bob@example.com", "secret123"};
 * authService->registerUser(req, [](const AuthResponse& response) {
 *     if (response.is_success) {
 *         // Utilisateur créé, token JWT disponible
 *         std::cout << "Token: " << response.token << std::endl;
 *     }
 * });
 * ```
 */

#pragma once
#include "dto/auth/LoginRequest.h"
#include "dto/auth/RegisterRequest.h"
#include "dto/auth/AuthResponse.h"
#include "repositories/UserRepository.h"
#include <functional>
#include <memory>
#include <vector>
#include <string>

namespace services {

/**
 * @class AuthService
 * @brief Service principal pour toutes les opérations d'authentification
 *
 * Cette classe encapsule toute la logique métier liée à l'authentification :
 * - Validation des données avant persistance
 * - Orchestration des différents composants (repo, crypto, jwt)
 * - Transformation des erreurs techniques en erreurs métier
 * - Application des règles business (unicité email/username, etc.)
 *
 * DESIGN PATTERNS UTILISÉS :
 * - Service Layer : logique métier centralisée
 * - Dependency Injection : UserRepository injecté via constructeur
 * - Async Callbacks : compatible avec l'architecture Drogon
 * - Single Responsibility : uniquement l'authentification
 *
 * THREAD SAFETY :
 * - Thread-safe : pas d'état mutable partagé
 * - Chaque méthode est indépendante
 * - Les dépendances (repository) sont thread-safe
 */
class AuthService {
private:
    /**
     * @brief Repository pour l'accès aux données utilisateur
     * 
     * Injecté via le constructeur pour faciliter les tests unitaires
     * et respecter l'inversion de dépendance (SOLID).
     */
    std::shared_ptr<repositories::UserRepository> userRepo_;

public:
    /**
     * @brief Constructeur du service d'authentification
     * 
     * Initialise le service avec ses dépendances. Pattern d'injection
     * de dépendance pour faciliter les tests et découplage.
     * 
     * @param userRepo Repository pour l'accès aux données utilisateur
     * 
     * EXEMPLE D'INSTANCIATION :
     * ```cpp
     * auto dbClient = app().getDbClient();
     * auto userRepo = std::make_shared<UserRepository>(dbClient);
     * auto authService = std::make_shared<AuthService>(userRepo);
     * ```
     * 
     * @note Le repository doit être thread-safe
     * @note Utilise shared_ptr pour gestion automatique mémoire
     */
    explicit AuthService(std::shared_ptr<repositories::UserRepository> userRepo);

    /**
     * @brief Inscrit un nouvel utilisateur dans le système
     * 
     * PROCESSUS COMPLET D'INSCRIPTION :
     * 
     * 1. **VALIDATION PRÉALABLE**
     *    - Validation du DTO (format email, longueur password, etc.)
     *    - Vérification unicité username et email
     * 
     * 2. **SÉCURITÉ**
     *    - Hash du mot de passe avec BCrypt (salt aléatoire)
     *    - Création de l'entité User sécurisée
     * 
     * 3. **PERSISTANCE**
     *    - Insertion en base via UserRepository
     *    - Gestion des erreurs de contraintes DB
     * 
     * 4. **AUTHENTIFICATION**
     *    - Génération token JWT pour connexion immédiate
     *    - Préparation réponse avec données utilisateur (sans password)
     * 
     * @param request DTO contenant les données d'inscription validées
     * @param callback Fonction appelée avec le résultat (succès ou erreur)
     * 
     * EXEMPLE D'UTILISATION :
     * ```cpp
     * RegisterRequest req;
     * req.username = "alice_martin";
     * req.email = "alice@example.com";
     * req.password = "motDePasseSecurise123";
     * 
     * authService->registerUser(req, [](const AuthResponse& response) {
     *     if (response.is_success) {
     *         LOG("User registered: " + response.user.username);
     *         LOG("JWT Token: " + response.token);
     *         // Rediriger vers tableau de bord
     *     } else {
     *         LOG("Registration failed: " + response.message);
     *         // Afficher erreurs à l'utilisateur
     *     }
     * });
     * ```
     * 
     * RÉPONSE SUCCÈS :
     * ```cpp
     * AuthResponse {
     *   is_success = true,
     *   message = "Registration successful",
     *   token = "eyJhbGciOiJIUzI1NiIs...",
     *   user = {
     *     id = 42,
     *     username = "alice_martin",
     *     email = "alice@example.com",
     *     is_active = true,
     *     created_at = "2025-07-08 14:30:15"
     *   }
     * }
     * ```
     * 
     * RÉPONSE ERREUR :
     * ```cpp
     * AuthResponse {
     *   is_success = false,
     *   message = "Registration failed: Username already exists"
     * }
     * ```
     * 
     * ERREURS POSSIBLES :
     * - Username déjà utilisé
     * - Email déjà utilisé  
     * - Données invalides (email malformé, password trop court)
     * - Erreur technique base de données
     * 
     * @note Callback appelé dans TOUS les cas (succès ou erreur)
     * @note Thread-safe : peut être appelé depuis plusieurs threads
     * @note Async : ne bloque jamais le thread appelant
     */
    void registerUser(const dto::auth::RegisterRequest& request,
                     std::function<void(const dto::auth::AuthResponse&)> callback);

    /**
     * @brief Authentifie un utilisateur existant
     * 
     * PROCESSUS COMPLET DE CONNEXION :
     * 
     * 1. **RECHERCHE UTILISATEUR**
     *    - Recherche par username dans la base
     *    - Vérification que l'utilisateur est actif
     * 
     * 2. **VÉRIFICATION CREDENTIALS**
     *    - Comparaison password fourni vs hash stocké (BCrypt)
     *    - Protection contre les attaques par timing
     * 
     * 3. **AUTHENTIFICATION**
     *    - Génération token JWT avec infos utilisateur
     *    - Préparation réponse avec données utilisateur (sans password)
     * 
     * @param request DTO contenant username et password
     * @param callback Fonction appelée avec le résultat (succès ou erreur)
     * 
     * EXEMPLE D'UTILISATION :
     * ```cpp
     * LoginRequest req;
     * req.username = "alice_martin"; // ou email selon implémentation
     * req.password = "motDePasseSecurise123";
     * 
     * authService->login(req, [](const AuthResponse& response) {
     *     if (response.is_success) {
     *         // Stocker le token JWT (localStorage, cookie sécurisé, etc.)
     *         storeToken(response.token);
     *         redirectToDashboard();
     *     } else {
     *         showError("Identifiants incorrects");
     *     }
     * });
     * ```
     * 
     * RÉPONSE SUCCÈS :
     * ```cpp
     * AuthResponse {
     *   is_success = true,
     *   message = "Authentication successful",
     *   token = "eyJhbGciOiJIUzI1NiIs...",
     *   user = { / données utilisateur / }
     * }
     * ```
     * 
     * RÉPONSE ERREUR :
     * ```cpp
     * AuthResponse {
     *   is_success = false,
     *   message = "Invalid credentials"
     * }
     * ```
     * 
     * SÉCURITÉ :
     * - Message d'erreur générique (ne révèle pas si username ou password faux)
     * - Protection contre brute force (à implémenter côté middleware)
     * - Logging des tentatives de connexion pour audit
     * 
     * @note Callback appelé dans TOUS les cas (succès ou erreur)
     * @note Authentification réussie = token JWT valide pendant 24h
     * @note Thread-safe et async
     */
    void login(const dto::auth::LoginRequest& request,
              std::function<void(const dto::auth::AuthResponse&)> callback);

    /**
     * @brief Vérifie si un utilisateur peut s'inscrire avec les données fournies
     * 
     * Cette méthode effectue TOUTES les validations nécessaires avant inscription :
     * 
     * 1. **VALIDATION DTO**
     *    - Format email (regex)
     *    - Longueur et caractères autorisés pour username
     *    - Complexité mot de passe (longueur minimale)
     * 
     * 2. **VALIDATION MÉTIER**
     *    - Unicité du username (vérification async en base)
     *    - Unicité de l'email (vérification async en base)
     * 
     * @param request DTO d'inscription à valider
     * @param callback Fonction appelée avec (canRegister, errors)
     * 
     * EXEMPLE D'UTILISATION :
     * ```cpp
     * RegisterRequest req{"bob123", "invalid-email", "123"}; // données invalides
     * 
     * authService->canRegister(req, [](bool canRegister, std::vector<std::string> errors) {
     *     if (canRegister) {
     *         proceedWithRegistration();
     *     } else {
     *         for (const auto& error : errors) {
     *             showValidationError(error);
     *         }
     *     }
     * });
     * ```
     * 
     * EXEMPLES DE RETOUR :
     * 
     * CAS VALIDE :
     * ```cpp
     * callback(true, {}); // Aucune erreur, peut s'inscrire
     * ```
     * 
     * CAS INVALIDE :
     * ```cpp
     * callback(false, {
     *     "Invalid email format",
     *     "Password must be at least 6 characters long",
     *     "Username already exists"
     * });
     * ```
     * 
     * TYPES D'ERREURS DÉTECTÉES :
     * - Email malformé (regex validation)
     * - Username trop court/long ou caractères interdits
     * - Password trop simple (longueur insuffisante)
     * - Username déjà pris par un autre utilisateur
     * - Email déjà utilisé par un autre compte
     * 
     * @note Utilisée en interne par registerUser(), mais peut être exposée publiquement
     * @note Optimisation possible : cache des usernames/emails pour éviter requêtes DB répétées
     * @note Thread-safe et async
     */
private:
    void canRegister(const dto::auth::RegisterRequest& request,
                    std::function<void(bool, std::vector<std::string>)> callback);
};

} // namespace services