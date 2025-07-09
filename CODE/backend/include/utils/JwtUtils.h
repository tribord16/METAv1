// ============================================================================
// include/utils/JwtUtils.h - JWT Token Management (Continued)
// ============================================================================

#pragma once
#include <jwt-cpp/jwt.h>
#include <string>
#include <chrono>
#include <optional>
#include "models/User.h"
#include "Logger.h"

namespace utils {

struct JwtPayload {
    int user_id;
    std::string username;
    std::string email;
    std::chrono::system_clock::time_point expires_at;
};

class JwtUtils {
private:
    static inline const std::string SECRET_KEY = "your-super-secret-jwt-key-change-in-production";
    static inline const std::chrono::hours TOKEN_LIFETIME{24};

public:
    // Generate JWT token for authenticated user
    static std::string generateToken(const models::User& user) {
        auto now = std::chrono::system_clock::now();
        auto expires = now + TOKEN_LIFETIME;
        
        try {
            auto token = jwt::create()
                .set_issuer("metaleague-backend")
                .set_type("JWT")
                .set_issued_at(now)
                .set_expires_at(expires)
                .set_payload_claim("user_id", jwt::claim(std::to_string(user.id)))
                .set_payload_claim("username", jwt::claim(user.username))
                .set_payload_claim("email", jwt::claim(user.email))
                .sign(jwt::algorithm::hs256{SECRET_KEY});
            
            Logger::debug("JWT token generated for user: " + user.username);
            return token;
        } catch (const std::exception& e) {
            Logger::error("Failed to generate JWT token: " + std::string(e.what()));
            return "";
        }
    }

    // Verify and decode JWT token
    static std::optional<JwtPayload> verifyToken(const std::string& token) {
        try {
            auto verifier = jwt::verify()
                .allow_algorithm(jwt::algorithm::hs256{SECRET_KEY})
                .with_issuer("metaleague-backend");
            
            auto decoded = jwt::decode(token);
            verifier.verify(decoded);
            
            JwtPayload payload;
            // Correction: Utiliser as_string() puis std::stoi pour récupérer l'id utilisateur
            payload.user_id = std::stoi(decoded.get_payload_claim("user_id").as_string());
            payload.username = decoded.get_payload_claim("username").as_string();
            payload.email = decoded.get_payload_claim("email").as_string();
            payload.expires_at = decoded.get_expires_at();
            
            return payload;
        } catch (const std::exception& e) {
            Logger::debug("JWT verification failed: " + std::string(e.what()));
            return std::nullopt;
        }
    }

    // Extract token from Authorization header
    static std::string extractTokenFromHeader(const std::string& authHeader) {
        const std::string bearer = "Bearer ";
        if (authHeader.length() > bearer.length() && 
            authHeader.substr(0, bearer.length()) == bearer) {
            return authHeader.substr(bearer.length());
        }
        return "";
    }
};

} // namespace utils
