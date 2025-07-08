// src/middlewares/RateLimitFilter.cpp
#include "middlewares/RateLimitFilter.h"
#include <json/json.h>

namespace middlewares {

// Définitions statiques (IMPORTANT - sinon erreur de linkage)
std::unordered_map<std::string, RateLimitEntry> RateLimitFilter::rate_limit_map_;
std::mutex RateLimitFilter::rate_limit_mutex_;

void RateLimitFilter::doFilter(const drogon::HttpRequestPtr &req,
                              drogon::FilterCallback &&fcb,
                              drogon::FilterChainCallback &&fccb) {
    
    std::string client_ip = req->peerAddr().toIp();
    std::string path = req->path();
    
    Logger::info("[RateLimitFilter] Checking " + client_ip + " -> " + path);
    
    if (isRateLimited(client_ip)) {
        Logger::warn("[RateLimitFilter] RATE LIMITED: " + client_ip);
        
        auto resp = drogon::HttpResponse::newHttpResponse();
        resp->setStatusCode(drogon::k429TooManyRequests);
        
        Json::Value error;
        error["success"] = false;
        error["message"] = "Rate limit exceeded. Try again later.";
        error["retry_after"] = 60;
        
        resp->setBody(error.toStyledString());
        resp->setContentTypeCode(drogon::CT_APPLICATION_JSON);
        resp->addHeader("Retry-After", "60");
        
        fcb(resp);  // BLOQUER = pas de fccb()
        return;
    }
    
    Logger::debug("[RateLimitFilter] Request allowed for " + client_ip);
    fccb(); // CONTINUER
}

bool RateLimitFilter::isRateLimited(const std::string &client_ip) {
    std::lock_guard<std::mutex> lock(rate_limit_mutex_);
    
    auto now = std::chrono::steady_clock::now();
    auto it = rate_limit_map_.find(client_ip);
    
    if (it == rate_limit_map_.end()) {
        // Première requête de cette IP
        rate_limit_map_[client_ip] = {1, now};
        Logger::debug("[RateLimitFilter] First request from " + client_ip);
        return false;
    }
    
    auto &entry = it->second;
    auto time_diff = std::chrono::duration_cast<std::chrono::minutes>(now - entry.window_start);
    
    if (time_diff.count() >= 1) {
        // Nouvelle fenêtre d'1 minute
        entry.requests = 1;
        entry.window_start = now;
        Logger::debug("[RateLimitFilter] New time window for " + client_ip);
        return false;
    }
    
    // VÉRIFIER AVANT D'INCRÉMENTER ! ← LE FIX
    if (entry.requests >= MAX_REQUESTS_PER_MINUTE) {
        Logger::warn("[RateLimitFilter] Limit exceeded: " + 
                    std::to_string(entry.requests) + "/" + std::to_string(MAX_REQUESTS_PER_MINUTE));
        return true; // BLOQUER
    }
    
    // Seulement maintenant on incrémente
    entry.requests++;
    Logger::debug("[RateLimitFilter] IP " + client_ip + " requests: " + 
                  std::to_string(entry.requests) + "/" + std::to_string(MAX_REQUESTS_PER_MINUTE));
    
    return false; // AUTORISER
}

} // namespace middlewares