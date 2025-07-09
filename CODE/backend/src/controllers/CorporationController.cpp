
/************************************************************
 * @file CorporationController.cpp
 * @brief Implémentation du contrôleur API pour la gestion des corporations
 *
 * Rôle :
 *   - Gérer toutes les routes REST liées aux corporations (CRUD, dashboard)
 *   - Orchestrer la logique métier via CorporationService
 *   - Valider les requêtes et retourner des réponses formatées
 *
 * Place dans l'architecture :
 *   - Contrôleur HTTP principal pour le domaine "corporation"
 *   - Appelé automatiquement par Drogon via la macro PATH_LIST
 *
 * Dépendances :
 *   - services/CorporationService (logique métier)
 *   - models/Corporation (modèle de données)
 *   - dto/corporation (contrats API)
 *   - middlewares/JwtMiddleware, RateLimitMiddleware (sécurité)
 *
 * TODO :
 *   - Ajouter des validations avancées sur les entrées (nom, unicité, etc.)
 *   - Ajouter des logs d'accès et de modification
 *   - Ajouter des tests unitaires sur chaque handler
 *   - Factoriser la gestion des erreurs et des permissions
 ************************************************************/

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

    // TODO: Ajouter validation avancée (unicité du nom, format, etc.)
    // TODO: Logger la création de corporation

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
        // TODO: Logger l'accès à la liste des corporations
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
    /*Logger::info("[CorporationController] OPTIONS request handled directly");
    auto resp = HttpResponse::newHttpResponse();
    resp->setStatusCode(HttpStatusCode::k200OK);
    // CORS headers dynamiques pour compatibilité credentials
    std::string origin = req->getHeader("origin");
    if (origin.empty()) origin = "http://localhost:3000";
    resp->addHeader("Access-Control-Allow-Origin", origin);
    resp->addHeader("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
    resp->addHeader("Access-Control-Allow-Headers", "Content-Type, Authorization, X-Requested-With");
    resp->addHeader("Access-Control-Allow-Credentials", "true");
    resp->addHeader("Access-Control-Max-Age", "86400");
    resp->setBody("");
    callback(resp);*/
}