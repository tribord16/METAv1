

/************************************************************
 * @file CorsMiddleware.cpp
 * @brief Implémentation du middleware CORS global (Cross-Origin Resource Sharing)
 *
 * Rôle :
 *   - Ajouter les headers CORS à toutes les réponses HTTP
 *   - Gérer les requêtes OPTIONS (preflight) automatiquement
 *   - Permettre aux applications web de consommer l'API en cross-domain
 *
 * Place dans l'architecture :
 *   - Middleware global, déclaré dans config.json
 *   - S'applique à tous les endpoints de l'API
 *
 * Dépendances :
 *   - Drogon (HttpMiddleware)
 *   - utils/Logger (logs)
 *
 * TODO :
 *   - Restreindre les origins autorisés en production
 *   - Ajouter des tests unitaires sur les cas CORS
 *   - Logger les origines et méthodes pour audit
 ************************************************************/


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

    // Fonction utilitaire pour ajouter les headers CORS à une réponse
    auto addCorsHeaders = [&req](const HttpResponsePtr& resp) {
        std::string origin = req->getHeader("origin");
        if (origin.empty()) origin = "http://localhost:3000";
        resp->addHeader("Access-Control-Allow-Origin", origin);
        resp->addHeader("Access-Control-Allow-Credentials", "true");
        resp->addHeader("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
        resp->addHeader("Access-Control-Allow-Headers", "Content-Type, Authorization, X-Requested-With");
        resp->addHeader("Access-Control-Max-Age", "86400");
        Logger::debug("[CorsMiddleware] Headers CORS ajoutés à la réponse pour origin: " + origin);
    };

    // ==================== CAS 1 : REQUÊTE PREFLIGHT (OPTIONS) ====================
    if (req->getMethod() == HttpMethod::Options) {
        Logger::info(std::string("[CorsMiddleware] Requête OPTIONS interceptée pour ") + req->path());
        // TODO: Logger l'origine et l'User-Agent pour audit
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

