
/**
 * @file AuthController.cpp
 * @brief Implémentation du contrôleur d'authentification (routes /api/auth)
 * @author Meta League Backend Team
 * @date 2025
 *
 * RESPONSABILITÉS :
 * - Gérer toutes les routes d'authentification (inscription, connexion, profil utilisateur courant)
 * - Valider les requêtes et orchestrer la logique métier via AuthService
 *
 * UTILISATION :
 * - Instancié automatiquement par Drogon via la macro PATH_LIST dans AuthController.h
 * - Ne gère QUE les routes /api/auth
 *
 * DÉPENDANCES :
 * - services/AuthService (logique métier)
 * - repositories/UserRepository (accès DB)
 * - dto/auth/(contrats API)
 * - middlewares/JwtMiddleware (protection JWT)
 */

#include "controllers/AuthController.h"
#include "dto/auth/LoginRequest.h"
#include "dto/auth/RegisterRequest.h"
#include "dto/common/ApiResponse.h"
#include "repositories/UserRepository.h"
#include "utils/Logger.h"
#include <drogon/drogon.h>

using namespace drogon;
using namespace controllers;


/**
 * @class AuthController
 * @brief Contrôleur Drogon pour l'authentification (HttpSimpleController)
 *
 * Cette classe gère toutes les routes d'authentification :
 *   - POST /api/auth/register : inscription
 *   - POST /api/auth/login    : connexion
 *   - GET  /api/auth/me       : infos utilisateur courant (JWT)
 *
 * EXEMPLE D'UTILISATION :
 * ```cpp
 * // Appel automatique par Drogon via asyncHandleHttpRequest
 * // Pas d'instanciation manuelle
 * ```
 *
 * NOTES :
 * - Uniquement pour l'auth, pas d'autres domaines métier !
 * - Thread-safe (stateless)
 * - Utilise la séparation des responsabilités (service, repo, dto)
 */
AuthController::AuthController() {
    Logger::info("[AuthController] Instancié !");
    // Initialiser le service avec le repository
    auto db = app().getDbClient();
    auto userRepo = std::make_shared<repositories::UserRepository>(db);
    authService_ = std::make_shared<services::AuthService>(userRepo);
}



/**
 * @brief Point d'entrée unique Drogon pour toutes les routes de ce controller
 * @param req Requête HTTP reçue
 * @param callback Callback pour renvoyer la réponse HTTP
 *
 * NOTES :
 * - Route la requête vers le bon handler privé selon le chemin
 * - Ne gère QUE les routes déclarées dans PATH_LIST (voir AuthController.h)
 */
void AuthController::asyncHandleHttpRequest(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) {
    const auto& path = req->path();
    if (path == "/api/auth/register") {
        handleRegister(req, std::move(callback));
    } else if (path == "/api/auth/login") {
        handleLogin(req, std::move(callback));
    } else if (path == "/api/auth/me") {
        handleMe(req, std::move(callback));
    } else {
        callback(createErrorResponse("Not found", HttpStatusCode::k404NotFound));
    }
}


/**
 * @brief Handler pour l'inscription utilisateur
 * @param req Requête HTTP POST /api/auth/register (JSON RegisterRequest)
 * @param callback Callback pour renvoyer la réponse HTTP
 *
 * EXEMPLE :
 * ```bash
 * curl -X POST http://localhost:8080/api/auth/register -H "Content-Type: application/json" -d '{"username":"bob","email":"bob@ex.com","password":"azerty"}'
 * ```
 *
 * NOTES :
 * - Valide le JSON, appelle le service, renvoie la réponse API
 * - Retourne 201 si succès, 400 sinon
 */
void AuthController::handleRegister(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) {
    Logger::info("POST /api/auth/register");
    auto json = req->getJsonObject();
    if (!json) {
        callback(createErrorResponse("JSON body required"));
        return;
    }
    auto registerReq = dto::auth::RegisterRequest::fromJson(*json);
    if (!registerReq.isValid()) {
        callback(createValidationErrorResponse(registerReq.getErrors()));
        return;
    }
    authService_->registerUser(registerReq, [callback](const dto::auth::AuthResponse& authResp) {
        auto httpResp = HttpResponse::newHttpJsonResponse(authResp.toJson());
        httpResp->setStatusCode(authResp.is_success ? HttpStatusCode::k201Created : HttpStatusCode::k400BadRequest);
        callback(httpResp);
    });
}


/**
 * @brief Handler pour la connexion utilisateur
 * @param req Requête HTTP POST /api/auth/login (JSON LoginRequest)
 * @param callback Callback pour renvoyer la réponse HTTP
 *
 * EXEMPLE :
 * ```bash
 * curl -X POST http://localhost:8080/api/auth/login -H "Content-Type: application/json" -d '{"username":"bob","password":"azerty"}'
 * ```
 *
 * NOTES :
 * - Valide le JSON, appelle le service, renvoie la réponse API
 * - Retourne 200 si succès, 401 sinon
 */
void AuthController::handleLogin(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) {
    Logger::info("POST /api/auth/login");
    auto json = req->getJsonObject();
    if (!json) {
        callback(createErrorResponse("JSON body required"));
        return;
    }
    auto loginReq = dto::auth::LoginRequest::fromJson(*json);
    if (!loginReq.isValid()) {
        callback(createValidationErrorResponse(loginReq.getErrors()));
        return;
    }
    authService_->login(loginReq, [callback](const dto::auth::AuthResponse& authResp) {
        auto httpResp = HttpResponse::newHttpJsonResponse(authResp.toJson());
        httpResp->setStatusCode(authResp.is_success ? HttpStatusCode::k200OK : HttpStatusCode::k401Unauthorized);
        callback(httpResp);
    });
}


/**
 * @brief Handler pour récupérer les infos de l'utilisateur courant (JWT)
 * @param req Requête HTTP GET /api/auth/me (header Authorization: Bearer <token>)
 * @param callback Callback pour renvoyer la réponse HTTP
 *
 * EXEMPLE :
 * ```bash
 * curl -X GET http://localhost:8080/api/auth/me -H "Authorization: Bearer <token>"
 * ```
 *
 * NOTES :
 * - Nécessite un JWT valide (middleware JwtMiddleware)
 * - Retourne les infos de l'utilisateur extraites du token
 */
void AuthController::handleMe(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) {
    Logger::info("GET /api/auth/me");
    auto userId = req->attributes()->get<int>("user_id");
    auto username = req->attributes()->get<std::string>("username");
    auto email = req->attributes()->get<std::string>("email");
    Json::Value userData;
    userData["id"] = userId;
    userData["username"] = username;
    userData["email"] = email;
    auto response = dto::common::ApiResponse::success("User information retrieved", userData);
    auto httpResp = HttpResponse::newHttpJsonResponse(response.toJson());
    callback(httpResp);
}


// ==================== HELPERS ====================


/**
 * @brief Génère une réponse d'erreur API standard
 * @param message Message d'erreur à retourner
 * @param status Code HTTP à utiliser (par défaut 400)
 * @return HttpResponsePtr réponse JSON formatée
 *
 * NOTES :
 * - Utilisé pour toutes les erreurs simples
 */
HttpResponsePtr AuthController::createErrorResponse(const std::string& message, HttpStatusCode status) {
    auto response = dto::common::ApiResponse::error(message);
    auto httpResp = HttpResponse::newHttpJsonResponse(response.toJson());
    httpResp->setStatusCode(status);
    return httpResp;
}


/**
 * @brief Génère une réponse d'erreur de validation API
 * @param errors Liste des erreurs de validation
 * @return HttpResponsePtr réponse JSON formatée
 *
 * NOTES :
 * - Utilisé pour retourner les erreurs de validation de DTO
 */
HttpResponsePtr AuthController::createValidationErrorResponse(const std::vector<std::string>& errors) {
    auto response = dto::common::ApiResponse::error("Validation failed", errors);
    auto httpResp = HttpResponse::newHttpJsonResponse(response.toJson());
    httpResp->setStatusCode(HttpStatusCode::k400BadRequest);
    return httpResp;
}