
/************************************************************
 * @file JwtMiddleware.cpp
 * @brief Implémentation du middleware JWT (authentification et autorisation)
 *
 * Rôle :
 *   - Vérifier la présence et la validité des tokens JWT dans les requêtes HTTP
 *   - Extraire les informations utilisateur du token et les injecter dans la requête
 *   - Bloquer l'accès si le token est invalide ou absent
 *   - Gérer les erreurs d'authentification avec des réponses appropriées
 *
 * Place dans l'architecture :
 *   - Middleware de sécurité, utilisé dans les routes protégées (controllers)
 *   - S'exécute avant le contrôleur, court-circuite en cas d'échec
 *
 * Dépendances :
 *   - Drogon (HttpMiddleware)
 *   - services/JwtService (vérification cryptographique)
 *   - dto/common/ApiResponse (formatage des erreurs)
 *   - utils/Logger (logs)
 *
 * TODO :
 *   - Ajouter la gestion des rôles/permissions dans le payload
 *   - Logger les tentatives d'accès non autorisées avec plus de détails
 *   - Ajouter des tests unitaires sur tous les cas d'erreur
 ************************************************************/

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
    // Récupération du header Authorization (format attendu : "Authorization: Bearer <token>")
    auto authHeader = req->getHeader("Authorization");
    if (authHeader.empty()) {
        Logger::warn("[JwtMiddleware] Missing Authorization header for: " + req->path());
        // TODO: Logger l'adresse IP et l'User-Agent pour audit
        auto response = dto::common::ApiResponse::error("Missing Authorization header");
        auto httpResp = HttpResponse::newHttpJsonResponse(response.toJson());
        httpResp->setStatusCode(HttpStatusCode::k401Unauthorized);
        mcb(httpResp);
        return;
    }

    // ==================== ÉTAPE 2 : VALIDATION FORMAT ====================
    // Extraction du token depuis "Bearer <token>"
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
    // Vérification complète du token JWT :
    // - Signature HMAC-SHA256 avec la clé secrète
    // - Date d'expiration
    // - Format et issuer
    // - Extraction des claims utilisateur
    auto payload = services::JwtService::verifyToken(token);
    if (!payload.has_value()) {
        Logger::warn("[JwtMiddleware] Invalid or expired JWT token");
        // TODO: Logger l'adresse IP et l'User-Agent pour audit
        auto response = dto::common::ApiResponse::error("Invalid or expired token");
        auto httpResp = HttpResponse::newHttpJsonResponse(response.toJson());
        httpResp->setStatusCode(HttpStatusCode::k401Unauthorized);
        mcb(httpResp);
        return;
    }

    // ==================== ÉTAPE 4 : ENRICHISSEMENT REQUÊTE ====================
    // Injection des données utilisateur dans la requête
    // Le contrôleur pourra les récupérer via req->attributes()->get<T>("key")
    // DONNÉES DISPONIBLES : user_id (int), username (string), email (string)
    req->attributes()->insert("user_id", payload->user_id);
    req->attributes()->insert("username", payload->username);
    req->attributes()->insert("email", payload->email);

    Logger::info("[JwtMiddleware] JWT authentication successful for user: " + payload->username + 
                " (ID: " + std::to_string(payload->user_id) + ")");

    // ==================== ÉTAPE 5 : CONTINUATION VERS CONTRÔLEUR ====================
    // Token valide : on continue le traitement vers le contrôleur
    // nextCb() déclenche l'appel de la méthode du contrôleur
    // Le callback mcb est passé pour que le contrôleur puisse répondre
    nextCb([mcb = std::move(mcb)](const HttpResponsePtr& resp) {
        mcb(resp);
    });
}