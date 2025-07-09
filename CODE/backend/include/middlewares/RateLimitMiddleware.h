
/************************************************************
 * @file RateLimitMiddleware.h
 * @brief Middleware de limitation du taux de requêtes (anti-spam, anti-brute-force)
 *
 * Rôle :
 *   - Limiter le nombre de requêtes par IP (globale et endpoints sensibles)
 *   - Bannir temporairement les IPs après abus ou brute-force
 *   - Ajouter des headers informatifs de rate limiting
 *   - Nettoyer périodiquement les entrées obsolètes
 *
 * Place dans l'architecture :
 *   - Middleware de sécurité, utilisé sur les endpoints critiques (auth, etc.)
 *   - S'exécute avant le contrôleur, court-circuite en cas d'abus
 *
 * Dépendances :
 *   - Drogon (HttpMiddleware)
 *   - dto/common/ApiResponse (formatage des erreurs)
 *   - utils/Logger (logs)
 *
 * TODO :
 *   - Rendre la configuration dynamique (par fichier ou env)
 *   - Logger les tentatives de brute-force avec plus de détails (User-Agent, etc.)
 *   - Ajouter des tests unitaires sur tous les cas limites
 ************************************************************/

#pragma once
#include <drogon/HttpMiddleware.h>
#include <unordered_map>
#include <chrono>
#include <mutex>

namespace middlewares {

struct RateLimitInfo {
    int request_count;
    std::chrono::system_clock::time_point window_start;
    std::chrono::system_clock::time_point ban_until;
    int failed_attempts; // Pour auth endpoints spécifiquement
};

class RateLimitMiddleware : public drogon::HttpMiddleware<RateLimitMiddleware> {
private:
    static std::unordered_map<std::string, RateLimitInfo> ip_tracking_;
    static std::mutex tracking_mutex_;
    
    // Configuration
    static const int GLOBAL_REQUESTS_PER_HOUR = 1000;
    static const int AUTH_REQUESTS_PER_MINUTE = 5;
    static const int MAX_FAILED_AUTH_ATTEMPTS = 3;
    static const int BAN_DURATION_MINUTES;

public:
    RateLimitMiddleware();
    
    void invoke(const drogon::HttpRequestPtr& req,
                drogon::MiddlewareNextCallback&& nextCb,
                drogon::MiddlewareCallback&& mcb) override;

private:
    std::string getClientIp(const drogon::HttpRequestPtr& req);
    bool isAuthEndpoint(const std::string& path);
    bool checkRateLimit(const std::string& ip, const std::string& path);
    void updateRateLimit(const std::string& ip, const std::string& path, bool success = true);
    void addRateLimitHeaders(drogon::HttpResponsePtr& resp, const RateLimitInfo& info);
    void cleanupOldEntries();
};

} // namespace middlewares