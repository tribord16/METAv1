// ====== ./src/middlewares/JwtMiddleware.cpp ======
/**
 * @file JwtMiddleware.cpp
 * @brief Implémentation du middleware JWT avec gestion complète des erreurs
 */

#include "middlewares/JwtMiddleware.h"
#include "services/JwtService.h"
#include "dto/common/ApiResponse.h"
#include "utils/Logger.h"
#include <drogon/HttpResponse.h>

using namespace drogon;
using namespace middlewares;

void JwtMiddleware::invoke(const HttpRequestPtr& req,
                          MiddlewareNextCallback&& nextCb,
                          MiddlewareCallback&& mcb) {
    
    Logger::debug("[JwtMiddleware] Processing authentication for: " + req->path());
    
    // ==================== ÉTAPE 1 : EXTRACTION HEADER ====================
    
    /**
     * Récupération du header Authorization
     * Format attendu : "Authorization: Bearer <token>"
     */
    auto authHeader = req->getHeader("Authorization");
    if (authHeader.empty()) {
        Logger::warn("[JwtMiddleware] Missing Authorization header for: " + req->path());
        
        auto response = dto::common::ApiResponse::error("Missing Authorization header");
        auto httpResp = HttpResponse::newHttpJsonResponse(response.toJson());
        httpResp->setStatusCode(HttpStatusCode::k401Unauthorized);
        mcb(httpResp);
        return;
    }

    // ==================== ÉTAPE 2 : VALIDATION FORMAT ====================
    
    /**
     * Extraction du token depuis "Bearer <token>"
     * JwtService::extractTokenFromHeader() gère le parsing
     */
    std::string token = services::JwtService::extractTokenFromHeader(authHeader);
    if (token.empty()) {
        Logger::warn("[JwtMiddleware] Invalid Authorization header format: " + authHeader);
        
        auto response = dto::common::ApiResponse::error("Invalid Authorization header format");
        auto httpResp = HttpResponse::newHttpJsonResponse(response.toJson());
        httpResp->setStatusCode(HttpStatusCode::k401Unauthorized);
        mcb(httpResp);
        return;
    }

    // ==================== ÉTAPE 3 : VALIDATION CRYPTOGRAPHIQUE ====================
    
    /**
     * Vérification complète du token JWT :
     * - Signature HMAC-SHA256 avec la clé secrète
     * - Date d'expiration
     * - Format et issuer
     * - Extraction des claims utilisateur
     */
    auto payload = services::JwtService::verifyToken(token);
    if (!payload.has_value()) {
        Logger::warn("[JwtMiddleware] Invalid or expired JWT token");
        
        auto response = dto::common::ApiResponse::error("Invalid or expired token");
        auto httpResp = HttpResponse::newHttpJsonResponse(response.toJson());
        httpResp->setStatusCode(HttpStatusCode::k401Unauthorized);
        mcb(httpResp);
        return;
    }

    // ==================== ÉTAPE 4 : ENRICHISSEMENT REQUÊTE ====================
    
    /**
     * Injection des données utilisateur dans la requête
     * Le contrôleur pourra les récupérer via req->attributes()->get<T>("key")
     * 
     * DONNÉES DISPONIBLES :
     * - user_id (int) : identifiant unique de l'utilisateur
     * - username (string) : nom d'utilisateur
     * - email (string) : adresse email
     */
    req->attributes()->insert("user_id", payload->user_id);
    req->attributes()->insert("username", payload->username);
    req->attributes()->insert("email", payload->email);

    Logger::info("[JwtMiddleware] JWT authentication successful for user: " + payload->username + 
                " (ID: " + std::to_string(payload->user_id) + ")");

    // ==================== ÉTAPE 5 : CONTINUATION VERS CONTRÔLEUR ====================
    
    /**
     * Token valide : on continue le traitement vers le contrôleur
     * nextCb() déclenche l'appel de la méthode du contrôleur
     * Le callback mcb est passé pour que le contrôleur puisse répondre
     */
    nextCb([mcb = std::move(mcb)](const HttpResponsePtr& resp) {
        mcb(resp);
    });
}