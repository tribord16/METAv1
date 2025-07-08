// include/middlewares/RateLimitMiddleware.h
/**
 * @file RateLimitMiddleware.h
 * @brief Middleware de limitation du taux de requêtes pour éviter le spam et brute force
 * 
 * FONCTIONNALITÉS :
 * - Limite par IP (ex: 100 req/heure global, 5 login/minute)
 * - Bannissement temporaire après X tentatives
 * - Headers informatifs (X-RateLimit-Remaining, etc.)
 * - Configuration par endpoint
 * 
 * UTILISATION :
 * PATH_ADD("/api/auth/login", Post, "middlewares::RateLimitMiddleware");
 */

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