// src/middlewares/RateLimitMiddleware.cpp

#include <drogon/HttpAppFramework.h> // Pour DROGON_REGISTER_MIDDLEWARE
#include "middlewares/RateLimitMiddleware.h"
#include "dto/common/ApiResponse.h"
#include "utils/Logger.h"
#include <drogon/HttpResponse.h>

using namespace drogon;

namespace middlewares {

// Définitions statiques
std::unordered_map<std::string, RateLimitInfo> RateLimitMiddleware::ip_tracking_;
std::mutex RateLimitMiddleware::tracking_mutex_;
const int middlewares::RateLimitMiddleware::BAN_DURATION_MINUTES = 15; // ou la valeur que tu veux


// Constructeur
RateLimitMiddleware::RateLimitMiddleware() {
    Logger::info("[RateLimitMiddleware] Middleware de limitation de taux initialisé.");
}



void RateLimitMiddleware::invoke(const HttpRequestPtr& req,
                                MiddlewareNextCallback&& nextCb,
                                MiddlewareCallback&& mcb) {
    
    std::string clientIp = getClientIp(req);
    std::string path = req->path();
    
    Logger::debug("[RateLimit] Checking request from IP: " + clientIp + " to: " + path);
    
    // Nettoyer les anciennes entrées périodiquement
    static auto last_cleanup = std::chrono::system_clock::now();
    auto now = std::chrono::system_clock::now();
    if (now - last_cleanup > std::chrono::minutes(5)) {
        cleanupOldEntries();
        last_cleanup = now;
    }
    
    // Vérifier si l'IP est bannie
    {
        std::lock_guard<std::mutex> lock(tracking_mutex_);
        auto it = ip_tracking_.find(clientIp);
        if (it != ip_tracking_.end() && now < it->second.ban_until) {
            Logger::warn("[RateLimit] Blocked banned IP: " + clientIp);
            
            auto response = dto::common::ApiResponse::error("Too many failed attempts. Try again later.");
            auto httpResp = HttpResponse::newHttpJsonResponse(response.toJson());
            httpResp->setStatusCode(HttpStatusCode::k429TooManyRequests);
            httpResp->addHeader("Retry-After", "900"); // 15 minutes
            mcb(httpResp);
            return;
        }
    }
    
    // Vérifier les limites de taux
    if (!checkRateLimit(clientIp, path)) {
        Logger::warn("[RateLimit] Rate limit exceeded for IP: " + clientIp);
        
        auto response = dto::common::ApiResponse::error("Rate limit exceeded. Please slow down.");
        auto httpResp = HttpResponse::newHttpJsonResponse(response.toJson());
        httpResp->setStatusCode(HttpStatusCode::k429TooManyRequests);
        
        // Ajouter headers informatifs
        {
            std::lock_guard<std::mutex> lock(tracking_mutex_);
            auto it = ip_tracking_.find(clientIp);
            if (it != ip_tracking_.end()) {
                addRateLimitHeaders(httpResp, it->second);
            }
        }
        
        mcb(httpResp);
        return;
    }
    
    // Continuer vers le contrôleur et intercepter la réponse
    nextCb([this, clientIp, path, mcb = std::move(mcb)](const HttpResponsePtr& resp) {
        // Analyser la réponse pour détecter les échecs d'auth
        bool authSuccess = true;
        if (isAuthEndpoint(path) && resp->statusCode() == HttpStatusCode::k401Unauthorized) {
            authSuccess = false;
        }
        
        // Mettre à jour les compteurs
        updateRateLimit(clientIp, path, authSuccess);
        
        // Ajouter headers de rate limiting
        {
            std::lock_guard<std::mutex> lock(tracking_mutex_);
            auto it = ip_tracking_.find(clientIp);
            if (it != ip_tracking_.end()) {
                addRateLimitHeaders(const_cast<HttpResponsePtr&>(resp), it->second);
            }
        }
        
        mcb(resp);
    });
}

std::string RateLimitMiddleware::getClientIp(const HttpRequestPtr& req) {
    // Vérifier les headers de proxy en premier
    std::string ip = req->getHeader("X-Forwarded-For");
    if (!ip.empty()) {
        // Prendre la première IP si plusieurs
        size_t comma = ip.find(',');
        if (comma != std::string::npos) {
            ip = ip.substr(0, comma);
        }
        return ip;
    }
    
    ip = req->getHeader("X-Real-IP");
    if (!ip.empty()) {
        return ip;
    }
    
    // Fallback vers l'IP de la connexion
    return req->peerAddr().toIp();
}

bool RateLimitMiddleware::isAuthEndpoint(const std::string& path) {
    return path.find("/api/auth/") == 0 && 
           (path.find("/login") != std::string::npos || 
            path.find("/register") != std::string::npos);
}

bool RateLimitMiddleware::checkRateLimit(const std::string& ip, const std::string& path) {
    std::lock_guard<std::mutex> lock(tracking_mutex_);
    auto now = std::chrono::system_clock::now();
    
    auto& info = ip_tracking_[ip];
    
    // Réinitialiser la fenêtre si nécessaire
    auto window_duration = isAuthEndpoint(path) ? 
        std::chrono::minutes(1) : std::chrono::hours(1);
    
    if (now - info.window_start > window_duration) {
        info.window_start = now;
        info.request_count = 0;
    }
    
    // Vérifier les limites
    int limit = isAuthEndpoint(path) ? AUTH_REQUESTS_PER_MINUTE : GLOBAL_REQUESTS_PER_HOUR;
    
    return info.request_count < limit;
}

void RateLimitMiddleware::updateRateLimit(const std::string& ip, const std::string& path, bool success) {
    std::lock_guard<std::mutex> lock(tracking_mutex_);
    auto now = std::chrono::system_clock::now();
    
    auto& info = ip_tracking_[ip];
    info.request_count++;
    
    // Gérer les échecs d'authentification
    if (isAuthEndpoint(path) && !success) {
        info.failed_attempts++;
        Logger::warn("[RateLimit] Failed auth attempt #" + std::to_string(info.failed_attempts) + 
                    " for IP: " + ip);
        
        if (info.failed_attempts >= MAX_FAILED_AUTH_ATTEMPTS) {
            info.ban_until = now + std::chrono::minutes(BAN_DURATION_MINUTES);
            Logger::warn("[RateLimit] IP banned for " + std::to_string(BAN_DURATION_MINUTES) + 
                        " minutes: " + ip);
        }
    } else if (success) {
        // Reset les échecs en cas de succès
        info.failed_attempts = 0;
    }
}

void RateLimitMiddleware::addRateLimitHeaders(HttpResponsePtr& resp, const RateLimitInfo& info) {
    resp->addHeader("X-RateLimit-Limit", std::to_string(GLOBAL_REQUESTS_PER_HOUR));
    resp->addHeader("X-RateLimit-Remaining", 
                   std::to_string(std::max(0, GLOBAL_REQUESTS_PER_HOUR - info.request_count)));
    
    auto reset_time = std::chrono::duration_cast<std::chrono::seconds>(
        info.window_start.time_since_epoch()).count() + 3600; // +1 heure
    resp->addHeader("X-RateLimit-Reset", std::to_string(reset_time));
}

void RateLimitMiddleware::cleanupOldEntries() {
    std::lock_guard<std::mutex> lock(tracking_mutex_);
    auto now = std::chrono::system_clock::now();
    
    for (auto it = ip_tracking_.begin(); it != ip_tracking_.end();) {
        // Supprimer les entrées plus anciennes que 2 heures et non bannies
        if (now - it->second.window_start > std::chrono::hours(2) && 
            now >= it->second.ban_until) {
            it = ip_tracking_.erase(it);
        } else {
            ++it;
        }
    }
}

} // namespace middlewares
