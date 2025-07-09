/**
 * @file JwtService.cpp
 * @brief Implémentation JwtService - VERSION QUI MARCHE
 */

#include "services/JwtService.h"
#include "config/AppConfig.h"
#include "utils/Logger.h"
#include <jwt-cpp/jwt.h>
#include <chrono>

namespace services {

std::string JwtService::generateToken(const Users& user) {
    auto now = std::chrono::system_clock::now();
    auto exp = now + std::chrono::hours(24);
    
    try {
        auto token = jwt::create()
            .set_issuer("meta-league")
            .set_type("JWT")
            .set_payload_claim("user_id", jwt::claim(std::to_string(user.getValueOfId())))  // ← Convertir en string
            .set_payload_claim("username", jwt::claim(user.getValueOfUsername()))
            .set_payload_claim("email", jwt::claim(user.getValueOfEmail()))
            .set_issued_at(now)
            .set_expires_at(exp)
            .sign(jwt::algorithm::hs256{getSecret()});
        
        Logger::info("JWT token generated for user: " + user.getValueOfUsername());
        return token;
        
    } catch (const std::exception& e) {
        Logger::error("Error generating JWT token: " + std::string(e.what()));
        return "";
    }
}

std::optional<JwtPayload> JwtService::verifyToken(const std::string& token) {
    try {
        auto verifier = jwt::verify()
            .allow_algorithm(jwt::algorithm::hs256{getSecret()})
            .with_issuer("meta-league");
        
        auto decoded = jwt::decode(token);
        verifier.verify(decoded);
        
        JwtPayload payload;
        // Récupérer comme string puis convertir
        payload.user_id = std::stoi(decoded.get_payload_claim("user_id").as_string());
        payload.username = decoded.get_payload_claim("username").as_string();
        payload.email = decoded.get_payload_claim("email").as_string();
        
        return payload;
        
    } catch (const std::exception& e) {
        Logger::error("JWT verification failed: " + std::string(e.what()));
        return std::nullopt;
    }
}

std::string JwtService::extractTokenFromHeader(const std::string& authHeader) {
    const std::string bearer = "Bearer ";
    if (authHeader.substr(0, bearer.length()) == bearer) {
        return authHeader.substr(bearer.length());
    }
    return "";
}

std::string JwtService::getSecret() {
    return AppConfig::getJwtSecret();
}

} // namespace services