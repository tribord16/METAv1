#pragma once
/**
 * @file CorporationController.h
 * @brief Contrôleur API REST pour la gestion des corporations (Drogon HttpController)
 * @author MetaLeague Backend Team
 * @date 2025
 * @version 2.0
 *
 * ROUTES GÉRÉES :
 * - POST   /controllers/corporations                : Créer une corporation
 * - GET    /controllers/corporations                : Lister mes corporations
 * - GET    /controllers/corporations/{id}           : Détails d'une corporation
 * - GET    /controllers/corporations/{id}/dashboard : Dashboard complet
 * - PUT    /controllers/corporations/{id}           : Modifier une corporation
 * - DELETE /controllers/corporations/{id}           : Supprimer une corporation
 *
 * ARCHITECTURE :
 * - Hérite de drogon::HttpController<CorporationController>
 * - Utilise METHOD_LIST_BEGIN / METHOD_ADD pour déclarer les routes
 * - Chaque handler correspond à une route HTTP
 */

#include <drogon/HttpController.h>
#include "services/CorporationService.h"
#include <memory>

using namespace drogon;

namespace controllers {

class CorporationController : public drogon::HttpController<CorporationController> {
private:
    std::shared_ptr<services::CorporationService> corpService_;

public:
    CorporationController();

    METHOD_LIST_BEGIN
    METHOD_ADD(CorporationController::list, "/", Get, "middlewares::JwtMiddleware", "middlewares::RateLimitMiddleware", Options);
    METHOD_ADD(CorporationController::create, "/", Post, "middlewares::JwtMiddleware", "middlewares::RateLimitMiddleware", Options);
    METHOD_ADD(CorporationController::get, "/{1}", Get, "middlewares::JwtMiddleware", "middlewares::RateLimitMiddleware", Options);
    METHOD_ADD(CorporationController::update, "/{1}", Put, "middlewares::JwtMiddleware", "middlewares::RateLimitMiddleware", Options);
    METHOD_ADD(CorporationController::remove, "/{1}", Delete, "middlewares::JwtMiddleware", "middlewares::RateLimitMiddleware", Options);
    METHOD_ADD(CorporationController::dashboard, "/{1}/dashboard", Get, "middlewares::JwtMiddleware", "middlewares::RateLimitMiddleware", Options);
    METHOD_LIST_END

    // Handlers
    void create(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback);
    void list(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback);
    void get(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback, int corpId);
    void update(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback, int corpId);
    void remove(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback, int corpId);
    void dashboard(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback, int corpId);

    // Helpers
    std::shared_ptr<services::CorporationService> getCorporationService();
    HttpResponsePtr createErrorResponse(const std::string& message, HttpStatusCode status = HttpStatusCode::k400BadRequest);
    int getUserIdFromToken(const HttpRequestPtr& req);
    void handleOptions(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback);
};

} // namespace controllers