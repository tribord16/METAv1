// src/main.cpp - FIX CORS COMPLET


#include <drogon/drogon.h>
#include <drogon/orm/DbClient.h>
#include <controllers/AuthController.h>
#include <controllers/CorporationController.h>
#include <fstream>
#include <utils/Logger.h>
#include <config/AppConfig.h>
#include <dto/common/ApiResponse.h>

using namespace drogon;

int main() {
    try {
        // ==================== CONFIGURATION ====================
        AppConfig::init();
        Logger::info("Starting " + AppConfig::getAppName());
        Logger::info(std::string("Running in ") + (AppConfig::isDebug() ? "debug" : "production") + " mode");

        // ==================== CORS FIX COMPLET ====================
        
        /**
         * 🔧 PROBLÈME : Le navigateur envoie une requête OPTIONS (preflight)
         * AVANT chaque requête POST/PUT/DELETE avec headers custom
         * 
         * SOLUTION : Intercepter TOUTES les requêtes OPTIONS et répondre avec headers CORS
         */
        
        Logger::info("🔧 [CORS] Configuration CORS complète...");

        // 1. PREFLIGHT : Intercepter TOUTES les OPTIONS
        app().registerPreRoutingAdvice([](const HttpRequestPtr &req,
                                          FilterCallback &&stop,
                                          FilterChainCallback &&pass) {
            
            Logger::debug(std::string("[CORS] Request: ") + req->getMethodString() + " " + req->path());
            
            // Si c'est une requête OPTIONS (preflight), on répond directement
            if (req->method() == drogon::Options) {
                Logger::info("🔧 [CORS] Preflight OPTIONS interceptée pour: " + req->path());
                
                auto resp = drogon::HttpResponse::newHttpResponse();
                resp->setStatusCode(drogon::k200OK);
                
                // Headers CORS complets
                resp->addHeader("Access-Control-Allow-Origin", "*");
                resp->addHeader("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
                resp->addHeader("Access-Control-Allow-Headers", "Content-Type, Authorization, X-Requested-With");
                resp->addHeader("Access-Control-Max-Age", "86400"); // Cache 24h
                
                // Body vide pour OPTIONS
                resp->setBody("");
                
                stop(resp); // STOP = répondre directement, pas de controller
                return;
            }
            
            // Sinon, continuer normalement
            pass();
        });

        // 2. POST-HANDLING : Ajouter headers CORS à TOUTES les réponses
        app().registerPostHandlingAdvice([](const HttpRequestPtr &req, const HttpResponsePtr &resp) {
            // Ajouter headers CORS à toutes les réponses
            resp->addHeader("Access-Control-Allow-Origin", "*");
            resp->addHeader("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
            resp->addHeader("Access-Control-Allow-Headers", "Content-Type, Authorization, X-Requested-With");
            resp->addHeader("Access-Control-Max-Age", "86400");
            
            Logger::debug("[CORS] Headers ajoutés à la réponse: " + req->path() +
                          " | Method: " + req->getMethodString() +
                          " | Origin: " + req->getHeader("Origin") +
                          " | User-Agent: " + req->getHeader("User-Agent"));
        });

        Logger::info("✅ [CORS] CORS configuré : preflight OPTIONS + headers sur toutes réponses");

        // ==================== CONFIG DROGON ====================
        try {
            app().loadConfigFile("../config/config.json");
            Logger::info("✅ Config.json loaded");
        } catch (const std::exception& e) {
            Logger::warn("⚠️  Config.json not found, using defaults");
        }

        // ==================== SERVEUR CONFIG ====================
        app().setLogLevel(AppConfig::isDebug() ? trantor::Logger::kDebug : trantor::Logger::kInfo);
        app().addListener(AppConfig::getServerHost(), AppConfig::getServerPort());

        // ==================== ROUTES DE TEST ====================
        
        // Route test simple
        app().registerHandler("/test",
            [](const HttpRequestPtr &req, std::function<void (const HttpResponsePtr &)> &&callback) {
                Json::Value data;
                data["app"] = AppConfig::getAppName();
                data["debug"] = AppConfig::isDebug();
                data["server_time"] = std::time(nullptr);
                data["cors_test"] = "CORS headers should be present";
                auto apiResp = dto::common::ApiResponse::success("Test endpoint", data);
                auto response = HttpResponse::newHttpJsonResponse(apiResp.toJson());
                callback(response);
            });

        // Route test CORS spécifique
        app().registerHandler("/cors-test",
            [](const HttpRequestPtr &req, std::function<void (const HttpResponsePtr &)> &&callback) {
                Json::Value data;
                data["method"] = req->getMethodString();
                data["origin"] = req->getHeader("Origin");
                data["user_agent"] = req->getHeader("User-Agent");
                auto apiResp = dto::common::ApiResponse::success("CORS test endpoint", data);
                auto response = HttpResponse::newHttpJsonResponse(apiResp.toJson());
                callback(response);
            },
            {Get, Post, Put, Delete, Options}); // Multi-méthodes

        

        // ==================== LOGS DÉMARRAGE ====================
        Logger::info("🚀 Routes disponibles:");
        Logger::info("  📡 GET  /test                                     - Test config");
        Logger::info("  🔧 ANY  /cors-test                               - Test CORS");
        Logger::info("  🔐 POST /controllers/authcontroller/register     - User registration");
        Logger::info("  🔐 POST /controllers/authcontroller/login        - User login");
        Logger::info("  🔐 GET  /controllers/authcontroller/me           - Current user");
        Logger::info("  🏢 POST /controllers/corporationcontroller/      - Create corp");

        Logger::info("💡 Tests CORS à faire:");
        Logger::info("  1. curl -X OPTIONS http://localhost:8080/controllers/authcontroller/login");
        Logger::info("  2. Depuis ton frontend React sur localhost:3000");

        Logger::info("🌐 Server running on http://" + AppConfig::getServerHost() + ":" + std::to_string(AppConfig::getServerPort()));
        Logger::info("🔗 Frontend autorisé: http://localhost:3000 (et tous autres origins avec *)");

        // ==================== DÉMARRAGE ====================
        app().run();
        
    } catch (const std::exception& e) {
        std::cerr << "❌ [FATAL] " << e.what() << std::endl;
        Logger::error("Failed to start: " + std::string(e.what()));
        return 1;
    }
    return 0;
}