
/************************************************************
 * @file RateLimitFilter.h
 * @brief Filtre de limitation de taux (anti-spam simple)
 *
 * Rôle :
 *   - Limiter le nombre de requêtes par IP sur une fenêtre glissante (anti-spam)
 *   - Bloquer temporairement les IPs trop actives
 *   - Retourner une réponse 429 Too Many Requests si limite atteinte
 *
 * Place dans l'architecture :
 *   - Filtre HTTP, utilisé sur certains endpoints critiques
 *   - S'exécute avant le contrôleur, court-circuite en cas d'abus
 *
 * Dépendances :
 *   - Drogon (HttpFilter)
 *   - utils/Logger (logs)
 *
 * TODO :
 *   - Rendre la configuration dynamique (par fichier ou env)
 *   - Logger les tentatives de spam avec plus de détails (User-Agent, etc.)
 *   - Ajouter des tests unitaires sur tous les cas limites
 ************************************************************/

#pragma once
#include <drogon/HttpFilter.h>
#include <unordered_map>
#include <chrono>
#include <mutex>
#include <utils/Logger.h>

namespace middlewares {

struct RateLimitEntry {
    int requests;
    std::chrono::steady_clock::time_point window_start;
};

class RateLimitFilter : public drogon::HttpFilter<RateLimitFilter> {
private:
    static std::unordered_map<std::string, RateLimitEntry> rate_limit_map_;
    static std::mutex rate_limit_mutex_;
    static const int MAX_REQUESTS_PER_MINUTE = 5; // Plus strict pour les tests

public:
    RateLimitFilter() {
        Logger::info("[RateLimitFilter] Constructor called - Rate limiting active!");
    }

    void doFilter(const drogon::HttpRequestPtr &req,
                  drogon::FilterCallback &&fcb,
                  drogon::FilterChainCallback &&fccb) override;

private:
    bool isRateLimited(const std::string &client_ip);
};

} // namespace middlewares