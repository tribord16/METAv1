
// ====== ./src/middlewares/CorsMiddleware.cpp ======
/**
 * @file CorsMiddleware.cpp
 * @brief Implémentation complète du middleware CORS
 */


#include "middlewares/CorsMiddleware.h"
#include <drogon/HttpResponse.h>
#include "utils/Logger.h"

using namespace drogon;
using namespace middlewares;

CorsMiddleware::CorsMiddleware() {
    Logger::info("[CorsMiddleware] Middleware CORS initialisé.");
}
void CorsMiddleware::invoke(const HttpRequestPtr& req,
                           MiddlewareNextCallback&& nextCb,
                           MiddlewareCallback&& mcb) {
    static bool firstCall = true;
    if (firstCall) {
        Logger::info("[CorsMiddleware] Middleware CORS activé et prêt à intercepter les requêtes.");
        firstCall = false;
    }

    auto addCorsHeaders = [](const HttpResponsePtr& resp) {
        resp->addHeader("Access-Control-Allow-Origin", "*");
        resp->addHeader("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
        resp->addHeader("Access-Control-Allow-Headers", "Content-Type, Authorization, X-Requested-With");
        resp->addHeader("Access-Control-Max-Age", "86400");
        Logger::debug("[CorsMiddleware] Headers CORS ajoutés à la réponse.");
    };

    // ==================== CAS 1 : REQUÊTE PREFLIGHT (OPTIONS) ====================
    if (req->getMethod() == HttpMethod::Options) {
        Logger::info(std::string("[CorsMiddleware] Requête OPTIONS interceptée pour ") + req->path());
        auto resp = HttpResponse::newHttpResponse();
        resp->setStatusCode(HttpStatusCode::k200OK);
        addCorsHeaders(resp);
        mcb(resp);
        return;
    }

    // ==================== CAS 2 : REQUÊTES NORMALES ====================
    Logger::debug(std::string("[CorsMiddleware] Passage d'une requête normale (méthode : ") + req->getMethodString() + ", path : " + req->path() + ")");
    nextCb([mcb = std::move(mcb), addCorsHeaders](const HttpResponsePtr& resp) {
        addCorsHeaders(resp);
        mcb(resp);
    });
    }

