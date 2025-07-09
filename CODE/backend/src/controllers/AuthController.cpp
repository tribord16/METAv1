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
    // Ne pas initialiser authService_ ici car la DB n'est pas encore prête
    // L'initialisation se fera de manière lazy dans getAuthService()
}

// ==================== HANDLERS DE ROUTES ====================
/**
 * @brief Handler pour l'inscription utilisateur (POST /api/auth/register)
 * @param req Requête HTTP contenant les données d'inscription
 * @param callback Fonction de rappel pour retourner la réponse
 */
void AuthController::registerUser(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) {
    // Désérialiser le DTO d'inscription
    auto dto = req->getJsonObject();
    if (!dto) {
        callback(createErrorResponse("Invalid JSON body", HttpStatusCode::k400BadRequest));
        return;
    }
    dto::auth::RegisterRequest registerRequest;
    try {
        registerRequest = dto::auth::RegisterRequest::fromJson(*dto);
    } catch (const std::exception& e) {
        callback(createErrorResponse(std::string("Invalid request data: ") + e.what(), HttpStatusCode::k400BadRequest));
        return;
    }
    // Valider le DTO
    if (!registerRequest.isValid()) {
        auto errors = registerRequest.getErrors();
        callback(createValidationErrorResponse(errors));
        return;
    }

    // Appeler le service d'authentification pour l'inscription
    getAuthService()->registerUser(registerRequest, [this, callback](const dto::auth::AuthResponse& response) {
        if (response.is_success) {
            auto apiResp = dto::common::ApiResponse::success("User registered successfully", response.toJson());
            auto httpResp = HttpResponse::newHttpJsonResponse(apiResp.toJson());
            httpResp->setStatusCode(HttpStatusCode::k201Created);
            callback(httpResp);
        } else {
            callback(this->createErrorResponse(response.message, HttpStatusCode::k400BadRequest));
        }
    });

}

/**
 * @brief Handler pour la connexion utilisateur (POST /api/auth/login)
 * @param req Requête HTTP contenant les données de connexion
 * @param callback Fonction de rappel pour retourner la réponse
 */
void AuthController::login(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) {
    // Désérialiser le DTO de connexion
    auto dto = req->getJsonObject();
    if (!dto) {
        callback(createErrorResponse("Invalid JSON body", HttpStatusCode::k400BadRequest));
        return;
    }
    dto::auth::LoginRequest loginRequest;
    try {
        loginRequest = dto::auth::LoginRequest::fromJson(*dto);
    } catch (const std::exception& e) {
        callback(createErrorResponse(std::string("Invalid request data: ") + e.what(), HttpStatusCode::k400BadRequest));
        return;
    }
    // Valider le DTO
    if (!loginRequest.isValid()) {
        auto errors = loginRequest.getErrors();
        callback(createValidationErrorResponse(errors));
        return;
    }
    // Appeler le service d'authentification pour la connexion
    getAuthService()->login(loginRequest, [this, callback](const dto::auth::AuthResponse& response) {
        if (response.is_success) {
            auto apiResp = dto::common::ApiResponse::success("Login successful", response.toJson());
            auto httpResp = HttpResponse::newHttpJsonResponse(apiResp.toJson());
            httpResp->setStatusCode(HttpStatusCode::k200OK);
            callback(httpResp);
        } else {
            callback(this->createErrorResponse(response.message, HttpStatusCode::k401Unauthorized));
        }
    });
}

/**
 * @brief Handler pour récupérer le profil utilisateur courant (GET /api/auth/me)
 * @param req Requête HTTP (protégée par JWT)
 * @param callback Fonction de rappel pour retourner la réponse
 */
void AuthController::me(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) {
    // Récupérer l'ID utilisateur depuis les attributes (injecté par JwtMiddleware)
    auto attrs = req->getAttributes();
    if (!attrs || !attrs->find("user_id")) {
        callback(createErrorResponse("Unauthorized", HttpStatusCode::k401Unauthorized));
        return;
    }
    
    // Récupérer l'ID utilisateur avec le bon type
    int userId = attrs->get<int>("user_id");
    if (userId <= 0) {
        callback(createErrorResponse("Invalid user ID", HttpStatusCode::k400BadRequest));
        return;
    }

    // Ici, tu peux soit retourner les infos du token, soit requêter la base si besoin
    // Pour l'exemple, on retourne juste l'ID utilisateur
    Json::Value data;
    data["id"] = userId;
    auto apiResp = dto::common::ApiResponse::success("User profile retrieved", data);
    auto httpResp = HttpResponse::newHttpJsonResponse(apiResp.toJson());
    httpResp->setStatusCode(HttpStatusCode::k200OK);
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
    auto apiResp = dto::common::ApiResponse::error(message);
    auto httpResp = HttpResponse::newHttpJsonResponse(apiResp.toJson());
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
    auto apiResp = dto::common::ApiResponse::error("Validation failed", errors);
    auto httpResp = HttpResponse::newHttpJsonResponse(apiResp.toJson());
    httpResp->setStatusCode(HttpStatusCode::k400BadRequest);
    return httpResp;
}


void AuthController::handleOptions(const HttpRequestPtr& req,
                                  std::function<void(const HttpResponsePtr&)>&& callback) {
    Logger::info("[AuthController] OPTIONS request handled directly");
    
    auto resp = HttpResponse::newHttpResponse();
    resp->setStatusCode(HttpStatusCode::k200OK);
    
    // Headers CORS explicites
    resp->addHeader("Access-Control-Allow-Origin", "*");
    resp->addHeader("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
    resp->addHeader("Access-Control-Allow-Headers", "Content-Type, Authorization, X-Requested-With");
    resp->addHeader("Access-Control-Max-Age", "86400");
    
    // Body vide
    resp->setBody("");
    
    callback(resp);
}


// ==================== LAZY INITIALIZATION ====================
/**
 * @brief Initialise paresseusement le service d'authentification
 * @return Pointeur partagé vers le service d'authentification
 */
std::shared_ptr<services::AuthService> AuthController::getAuthService() {
    if (!authService_) {
        auto db = app().getDbClient();
        auto userRepo = std::make_shared<repositories::UserRepository>(db);
        authService_ = std::make_shared<services::AuthService>(userRepo);
        Logger::debug("[AuthController] AuthService initialisé paresseusement");
    }
    Logger::info("[AuthController] AuthService récupéré");
    return authService_;
}