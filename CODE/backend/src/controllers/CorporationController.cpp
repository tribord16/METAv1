/**
 * @file CorporationController.cpp
 * @brief Implémentation du contrôleur API pour la gestion des corporations
 * @author MetaLeague Backend Team
 * @date 2025
 * 
 * 
 * RESPONSABILITÉS :
 * - Gérer les routes d'API REST pour les corporations
 * - Orchestrer la logique métier via CorporationService
 * - Valider les requêtes et retourner des réponses formatées
 * 
 * ROUTES GÉRÉES :
 * - POST   /api/corporations                : Créer une corporation
 * - GET    /api/corporations                : Lister mes corporations
 * - GET    /api/corporations/{id}           : Détails d'une corporation
 * - GET    /api/corporations/{id}/dashboard : Dashboard complet
 * - PUT    /api/corporations/{id}           : Modifier une corporation
 * - DELETE /api/corporations/{id}           : Supprimer une corporation
 * ARCHITECTURE :
 * - Hérite de drogon::HttpController<CorporationController>
 * - Utilise METHOD_LIST_BEGIN / METHOD_ADD pour déclarer les routes
 * - Chaque handler correspond à une route HTTP
 * DÉPENDANCES :
 * - services/CorporationService : Logique métier pour les corporations
 * - models/Corporation : Modèle de données pour les corporations
 * - dto/corporation : Contrats d'API pour les requêtes/réponses
 *  UTILISATION :
 * ```cpp
 *  // Contrôleur auto-détecté par Drogon, pas d'instanciation
 *  // Les routes sont automatiquement enregistrées via METHOD_LIST
 *  // Exemple d'appel client :
 * curl -X POST http://localhost:8080/api/corporations \
 *     -H "Content-Type: application/json" \
 *    -d '{"name":"Team Quantum", "user_id":42, "esports_active":true}'
 * * ```
 * * BONNES PRATIQUES :
 * * - Thread-safe (stateless, pas de variables globales)
 * - Validation systématique des entrées via DTOs
 * - Gestion d'erreurs homogène avec codes HTTP appropriés
 * - Logging détaillé pour debugging et sécurité
 * - Séparation claire : contrôleur = orchestration, service = logique métier
 *  
 */

#include "controllers/CorporationController.h"
#include "utils/Logger.h"
#include "dto/common/ApiResponse.h"

using namespace drogon;
using namespace controllers;

/*
 * Implémentation des méthodes du contrôleur pour la gestion des corporations
 *
 * @class CorporationController
 * @brief Contrôleur API REST pour la gestion des corporations
 *
 * Ce contrôleur gère les opérations CRUD pour les corporations, en s'appuyant sur
 * les services et les DTOs pour assurer une séparation claire des responsabilités.
 *
 *
 */ 

CorporationController::CorporationController() {
    Logger::info("[CorporationController] Instancié !");
    // Ne pas initialiser corpService_ ici car la DB n'est pas encore prête
    // L'initialisation se fera de manière lazy dans getCorporationService()
}

void CorporationController::create(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) {
    // Extraire le corps de la requête et le convertir en DTO
    auto request = req->getJsonObject();
    if (!request) {
        callback(createErrorResponse("Invalid JSON body", HttpStatusCode::k400BadRequest));
        return;
    }

    dto::corporation::CreateCorporationRequest createRequest;
    try {
        createRequest = dto::corporation::CreateCorporationRequest::fromJson(*request);
    } catch (const std::exception& e) {
        callback(createErrorResponse(std::string("Invalid request data: ") + e.what(), HttpStatusCode::k400BadRequest));
        return;
    }

    // Récupérer l'ID utilisateur depuis le token JWT
    int userId = getUserIdFromToken(req);
    if (userId <= 0) {
        callback(createErrorResponse("Unauthorized", HttpStatusCode::k401Unauthorized));
        return;
    }

    // Appeler le service pour créer la corporation (DTO only)
    getCorporationService()->createCorporation(createRequest, userId, [callback](const dto::corporation::CorporationResponse& resp) {
        bool success = resp.id > 0;
        auto apiResp = success
            ? dto::common::ApiResponse::success("Corporation created", resp.toJson())
            : dto::common::ApiResponse::error("Failed to create corporation");
        auto response = HttpResponse::newHttpJsonResponse(apiResp.toJson());
        response->setStatusCode(success ? HttpStatusCode::k201Created : HttpStatusCode::k400BadRequest);
        callback(response);
    });
}

void CorporationController::list(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) {
    // Récupérer l'ID utilisateur depuis le token JWT
    int userId = getUserIdFromToken(req);
    if (userId <= 0) {
        callback(createErrorResponse("Unauthorized", HttpStatusCode::k401Unauthorized));
        return;
    }

    // Appeler le service pour lister les corporations de l'utilisateur (DTO only)
    getCorporationService()->listCorporationsByUser(userId, [callback](const std::vector<dto::corporation::CorporationResponse>& corporations) {
        Json::Value jsonArray(Json::arrayValue);
        for (const auto& corp : corporations) {
            jsonArray.append(corp.toJson());
        }
        auto apiResp = dto::common::ApiResponse::success("Corporations fetched", jsonArray);
        auto response = HttpResponse::newHttpJsonResponse(apiResp.toJson());
        response->setStatusCode(HttpStatusCode::k200OK);
        callback(response);
    });
}

void CorporationController::get(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback, int corpId) {
    // Récupérer l'ID utilisateur depuis le token JWT
    int userId = getUserIdFromToken(req);
    if (userId <= 0) {
        callback(createErrorResponse("Unauthorized", HttpStatusCode::k401Unauthorized));
        return;
    }

    // Appeler le service pour récupérer la corporation par ID (DTO only)
    getCorporationService()->getCorporationById(corpId, userId, [callback](const dto::corporation::CorporationResponse& resp) {
        bool found = resp.id > 0;
        auto apiResp = found
            ? dto::common::ApiResponse::success("Corporation found", resp.toJson())
            : dto::common::ApiResponse::error("Corporation not found");
        auto response = HttpResponse::newHttpJsonResponse(apiResp.toJson());
        response->setStatusCode(found ? HttpStatusCode::k200OK : HttpStatusCode::k404NotFound);
        callback(response);
    });
}

void CorporationController::update(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback, int corpId) {
    // Extraire le corps de la requête et le convertir en DTO
    auto request = req->getJsonObject();
    if (!request) {
        callback(createErrorResponse("Invalid JSON body", HttpStatusCode::k400BadRequest));
        return;
    }

    // TODO: Créer un DTO UpdateCorporationRequest si besoin, ici on simule avec CreateCorporationRequest pour l'exemple
    dto::corporation::CreateCorporationRequest updateRequest;
    try {
        updateRequest = dto::corporation::CreateCorporationRequest::fromJson(*request);
    } catch (const std::exception& e) {
        callback(createErrorResponse(std::string("Invalid request data: ") + e.what(), HttpStatusCode::k400BadRequest));
        return;
    }

    // Récupérer l'ID utilisateur depuis le token JWT
    int userId = getUserIdFromToken(req);
    if (userId <= 0) {
        callback(createErrorResponse("Unauthorized", HttpStatusCode::k401Unauthorized));
        return;
    }

    // Appeler le service pour mettre à jour la corporation
    getCorporationService()->updateCorporation(corpId, updateRequest, userId, [callback](const dto::corporation::CorporationResponse& resp) {
        bool updated = resp.id > 0;
        auto apiResp = updated
            ? dto::common::ApiResponse::success("Corporation updated", resp.toJson())
            : dto::common::ApiResponse::error("Failed to update corporation");
        auto response = HttpResponse::newHttpJsonResponse(apiResp.toJson());
        response->setStatusCode(updated ? HttpStatusCode::k200OK : HttpStatusCode::k400BadRequest);
        callback(response);
    });
}

void CorporationController::remove(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback, int corpId) {
    // Récupérer l'ID utilisateur depuis le token JWT
    int userId = getUserIdFromToken(req);
    if (userId <= 0) {
        callback(createErrorResponse("Unauthorized", HttpStatusCode::k401Unauthorized));
        return;
    }

    // Appeler le service pour supprimer la corporation
    getCorporationService()->deleteCorporation(corpId, userId, [callback](bool success) {
        auto apiResp = success
            ? dto::common::ApiResponse::success("Corporation deleted")
            : dto::common::ApiResponse::error("Failed to delete corporation");
        auto response = HttpResponse::newHttpJsonResponse(apiResp.toJson());
        response->setStatusCode(success ? HttpStatusCode::k200OK : HttpStatusCode::k400BadRequest);
        callback(response);
    });
}

void CorporationController::dashboard(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback, int corpId) {
    // Récupérer l'ID utilisateur depuis le token JWT
    int userId = getUserIdFromToken(req);
    if (userId <= 0) {
        callback(createErrorResponse("Unauthorized", HttpStatusCode::k401Unauthorized));
        return;
    }

    // Appeler le service pour récupérer le dashboard de la corporation
    getCorporationService()->getCorporationDashboard(corpId, userId, [callback](const dto::corporation::CorporationDashboard& dashboard) {
        auto apiResp = dto::common::ApiResponse::success("Corporation dashboard", dashboard.toJson());
        auto response = HttpResponse::newHttpJsonResponse(apiResp.toJson());
        response->setStatusCode(HttpStatusCode::k200OK);
        callback(response);
    });
}

// ==================== LAZY INITIALIZATION ====================
/**
 * @brief Initialise paresseusement le service de gestion des corporations
 * @return Pointeur partagé vers le service de corporations
 */
std::shared_ptr<services::CorporationService> CorporationController::getCorporationService() {
    if (!corpService_) {
        auto db = app().getDbClient();
        auto corpRepo = std::make_shared<repositories::CorporationRepository>(db);
        corpService_ = std::make_shared<services::CorporationService>(corpRepo);
        Logger::debug("[CorporationController] CorporationService initialisé paresseusement");
    }
    return corpService_;
}

int CorporationController::getUserIdFromToken(const HttpRequestPtr& req) {
    // Récupérer l'ID utilisateur depuis les attributs du token JWT
    auto userId = req->attributes()->get<int>("user_id");
    if (!userId) {
        Logger::error("User ID not found in request attributes");
        return -1; // Indiquer une erreur
    }
    return userId;
}


HttpResponsePtr CorporationController::createErrorResponse(const std::string& message, HttpStatusCode status) {
    auto apiResp = dto::common::ApiResponse::error(message);
    auto httpResp = HttpResponse::newHttpJsonResponse(apiResp.toJson());
    httpResp->setStatusCode(status);
    return httpResp;
}

void CorporationController::handleOptions(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) {
    Logger::info("[CorporationController] OPTIONS request handled directly");
    
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