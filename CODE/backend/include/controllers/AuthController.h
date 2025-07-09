#pragma once
/**
 * @file AuthController.h
 * @brief Contrôleur d'authentification (Drogon HttpController)
 * @author Meta League Backend Team
 * @date 2025
 * @version 2.0
 *
 * ROUTES GÉRÉES :
    POST /controllers/authcontroller/register  → AuthController::registerUser
    POST /controllers/authcontroller/login     → AuthController::login  
    GET  /controllers/authcontroller/me        → AuthController::me
 *
 * ARCHITECTURE :
 * - Hérite de drogon::HttpController<AuthController>
 * - Utilise METHOD_LIST_BEGIN / METHOD_ADD pour déclarer les routes
 * - Chaque handler correspond à une route HTTP
 */

#include <drogon/HttpController.h>
#include "services/AuthService.h"
#include <memory>

using namespace drogon;

namespace controllers {

class AuthController : public drogon::HttpController<AuthController> {
private:
    std::shared_ptr<services::AuthService> authService_;

public:
    AuthController();

    METHOD_LIST_BEGIN
    // Routes d'authentification
    METHOD_ADD(AuthController::registerUser, "/register", Post, Options);
    METHOD_ADD(AuthController::login, "/login", Post, Options);
    // Route protégée avec middleware (syntaxe à vérifier)
    METHOD_ADD(AuthController::me, "/me", Get, "middlewares::JwtMiddleware", Options);
    METHOD_LIST_END

    // ==================== HANDLERS DE ROUTES ====================
    /**
     * @brief Handler pour l'inscription utilisateur (POST /api/auth/register)
     */
    void registerUser(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback);

    /**
     * @brief Handler pour la connexion utilisateur (POST /api/auth/login)
     */
    void login(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback);

    /**
     * @brief Handler pour récupérer le profil utilisateur courant (GET /api/auth/me)
     */
    void me(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback);


    /**
     * @brief Handler pour les requêtes OPTIONS (CORS)
     * @param req Requête HTTP
     * @param callback Fonction de rappel pour retourner la réponse
     * 
     * Utilisé pour gérer les requêtes CORS préflight.
     */

    void handleOptions(const HttpRequestPtr& req,
                      std::function<void(const HttpResponsePtr&)>&& callback);

    // ==================== MÉTHODES UTILITAIRES ====================
    std::shared_ptr<services::AuthService> getAuthService();
    HttpResponsePtr createErrorResponse(const std::string& message, HttpStatusCode status = HttpStatusCode::k400BadRequest);
    HttpResponsePtr createValidationErrorResponse(const std::vector<std::string>& errors);
};

} // namespace controllers