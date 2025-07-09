// ============================================================================
// include/dto/auth/AuthResponse.h - Authentication Response DTO
// ============================================================================

#pragma once
#include <nlohmann/json.hpp>
#include <string>
#include "models/User.h"

namespace dto {
namespace auth {

struct AuthResponse {
    bool success;
    std::string message;
    std::string token;
    models::User user;

    AuthResponse(bool success, const std::string& message, 
                const std::string& token = "", const models::User& user = {})
        : success(success), message(message), token(token), user(user) {}

    static AuthResponse createSuccess(const std::string& token, const models::User& user) {
        return AuthResponse(true, "Authentication successful", token, user);
    }

    static AuthResponse createError(const std::string& message) {
        return AuthResponse(false, message);
    }

    nlohmann::json toJson() const {
        nlohmann::json json;
        json["success"] = success;
        json["message"] = message;
        json["timestamp"] = std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
        
        if (success && !token.empty()) {
            json["token"] = token;
            json["user"] = user.toJson();
        }
        
        return json;
    }
};

} // namespace auth
} // namespace dto