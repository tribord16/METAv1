// ====== ./include/middlewares/JwtMiddleware.h ======
/**
 * @file JwtMiddleware.h
 * @brief Middleware d'authentification JWT pour protection des routes
 * @author Meta League Backend Team
 * @date 2025
 * @version 1.0
 *
 * RESPONSABILITÉS :
 * - Vérifier la présence et validité des tokens JWT
 * - Extraire les informations utilisateur du token
 * - Injecter les données utilisateur dans la requête (req->attributes)
 * - Bloquer l'accès si token invalide/absent
 * - Gérer les erreurs d'authentification avec réponses appropriées
 *
 * ARCHITECTURE MIDDLEWARE DROGON :
 * ```
 * HTTP Request → JwtMiddleware::invoke() → Controller::method()
 *                      ↓ (si token invalide)
 *                HTTP 401 Unauthorized
 * ```
 *
 * PATTERN D'UTILISATION :
 * - Déclaré dans PATH_LIST des contrôleurs pour routes protégées
 * - Automatiquement exécuté par Drogon avant le contrôleur
 * - Utilise les callbacks Drogon pour chaîner ou court-circuiter
 *
 * SÉCURITÉ :
 * - Validation cryptographique du token (signature HMAC-SHA256)
 * - Vérification de l'expiration (timestamp)
 * - Protection contre les tokens malformés
 * - Logging des tentatives d'accès non autorisées
 *
 * EXEMPLE D'UTILISATION :
 * ```cpp
 * // Dans AuthController.h
 * PATH_LIST_BEGIN
 * PATH_ADD("/api/auth/me", Get, "middlewares::JwtMiddleware");
 * PATH_LIST_END
 * 
 * // Le middleware s'exécute automatiquement avant handleMe()
 * ```
 *
 * FLOW COMPLET :
 * 1. Client envoie : Authorization: Bearer eyJhbGciOiJIUzI1NiIs...
 * 2. Middleware extrait et valide le token
 * 3. Si valide : injecte user_id, username, email dans req->attributes()
 * 4. Appelle le contrôleur qui peut utiliser ces données
 * 5. Si invalide : retourne 401 sans appeler le contrôleur
 */

#pragma once
#include <drogon/HttpMiddleware.h>

namespace middlewares {

/**
 * @class JwtMiddleware
 * @brief Middleware Drogon pour authentification et autorisation JWT
 *
 * Ce middleware implémente le pattern HttpMiddleware<T> de Drogon qui :
 * - S'exécute automatiquement avant les contrôleurs protégés
 * - Peut court-circuiter le flux (retour 401 direct)
 * - Peut enrichir la requête pour le contrôleur suivant
 * - Gère les réponses asynchrones via callbacks
 *
 * INTEGRATION DROGON :
 * - Héritage de HttpMiddleware<JwtMiddleware> (CRTP)
 * - Méthode invoke() appelée automatiquement
 * - Utilisation des callbacks nextCb (continuer) et mcb (répondre)
 * - Thread-safe par design (stateless)
 *
 * GESTION DES CAS :
 * ✅ Token valide → enrichissement req + appel contrôleur
 * ❌ Token absent → 401 "Missing Authorization header"
 * ❌ Token malformé → 401 "Invalid Authorization header format"
 * ❌ Token expiré/invalide → 401 "Invalid or expired token"
 *
 * DONNÉES INJECTÉES (si succès) :
 * - req->attributes()->get<int>("user_id")
 * - req->attributes()->get<std::string>("username")
 * - req->attributes()->get<std::string>("email")
 */
class JwtMiddleware : public drogon::HttpMiddleware<JwtMiddleware> {
public:
    /**
     * @brief Constructeur par défaut
     * 
     * Middleware stateless : pas d'état à initialiser.
     * Chaque instance peut traiter plusieurs requêtes simultanément.
     */
    JwtMiddleware() = default;

    /**
     * @brief Point d'entrée principal du middleware JWT
     * 
     * Cette méthode est appelée automatiquement par Drogon pour chaque
     * requête vers une route protégée par ce middleware.
     * 
     * PROCESSUS COMPLET :
     * 
     * 1. **EXTRACTION DU TOKEN**
     *    - Récupération header "Authorization"
     *    - Vérification format "Bearer <token>"
     *    - Extraction de la partie token
     * 
     * 2. **VALIDATION CRYPTOGRAPHIQUE**
     *    - Vérification signature HMAC-SHA256
     *    - Contrôle date d'expiration
     *    - Validation de l'issuer et format
     * 
     * 3. **ENRICHISSEMENT REQUÊTE**
     *    - Extraction des claims (user_id, username, email)
     *    - Injection dans req->attributes() pour le contrôleur
     * 
     * 4. **DÉCISION DE ROUTAGE**
     *    - Si valide : nextCb() → appel du contrôleur
     *    - Si invalide : mcb(401) → réponse directe sans contrôleur
     * 
     * @param req Requête HTTP entrante avec headers
     * @param nextCb Callback pour continuer vers le contrôleur (si token valide)
     * @param mcb Callback pour répondre directement (si token invalide)
     * 
     * EXEMPLE DE REQUÊTE VALIDE :
     * ```
     * GET /api/auth/me HTTP/1.1
     * Authorization: Bearer eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9...
     * ```
     * 
     * RÉPONSE 401 (token invalide) :
     * ```json
     * {
     *   "success": false,
     *   "message": "Invalid or expired token"
     * }
     * ```
     * 
     * CONTRÔLEUR APPELÉ (token valide) :
     * ```cpp
     * void AuthController::handleMe(const HttpRequestPtr& req, ...) {
     *     auto userId = req->attributes()->get<int>("user_id");
     *     auto username = req->attributes()->get<std::string>("username");
     *     // userId et username sont disponibles ici !
     * }
     * ```
     * 
     * @note Thread-safe : peut traiter plusieurs requêtes simultanément
     * @note Async : utilise les callbacks Drogon, ne bloque jamais
     * @note Stateless : pas d'état partagé entre requêtes
     */
    void invoke(const drogon::HttpRequestPtr& req,
                drogon::MiddlewareNextCallback&& nextCb,
                drogon::MiddlewareCallback&& mcb) override;
};

} // namespace middlewares