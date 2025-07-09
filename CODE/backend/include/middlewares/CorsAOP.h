#include <drogon/drogon.h>
#include <drogon/HttpResponse.h>
#include "utils/Logger.h"

void registerCorsAOP() {
    using namespace drogon;
    Logger::info("[CorsAOP] Enregistrement du CORS AOP global...");
    
    // Pre-routing: gère les requêtes OPTIONS (preflight)
    app().registerPreRoutingAdvice([](const HttpRequestPtr &req,
                                      FilterCallback &&stop,
                                      FilterChainCallback &&pass) {
        std::string origin = req->getHeader("origin");
        if (origin.empty()) origin = "http://localhost:3000";

        Logger::debug(std::string("[CorsAOP] Requête reçue: ") + req->getMethodString() + " " + req->path() + " Origin: " + origin);

        if (req->method() == Options) {
            Logger::info(std::string("[CorsAOP] Requête OPTIONS interceptée pour: ") + req->path());
            auto resp = HttpResponse::newHttpResponse();
            resp->addHeader("Access-Control-Allow-Origin", "*");  // TEMPORAIRE : * au lieu de origin
            resp->addHeader("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
            resp->addHeader("Access-Control-Allow-Headers", "Content-Type, Authorization, X-Requested-With");
            resp->addHeader("Access-Control-Max-Age", "86400");
            stop(resp);
            return;
        }
        pass();
    });

    // Post-handling: ajoute les headers CORS à toutes les réponses
    app().registerPostHandlingAdvice([](const HttpRequestPtr &req, const HttpResponsePtr &resp) {
        resp->addHeader("Access-Control-Allow-Origin", "*");  // TEMPORAIRE : * au lieu de origin
        resp->addHeader("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
        resp->addHeader("Access-Control-Allow-Headers", "Content-Type, Authorization, X-Requested-With");
        resp->addHeader("Access-Control-Max-Age", "86400");
        Logger::debug(std::string("[CorsAOP] Headers CORS ajoutés pour: ") + req->path());
    });
    
    Logger::info("[CorsAOP] ✅ CORS AOP configuré avec succès !");
}