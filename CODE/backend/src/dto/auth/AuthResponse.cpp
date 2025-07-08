/**
 * @file AuthResponse.cpp
 * @brief Implémentation AuthResponse
 */

#include "dto/auth/AuthResponse.h"

namespace dto {
namespace auth {

AuthResponse::AuthResponse(bool success, const std::string& message)
    : is_success(success), message(message) {}

AuthResponse::AuthResponse(bool success, const std::string& message, const std::string& token, const user::UserResponse& user)
    : is_success(success), message(message), token(token), user(user) {}

AuthResponse AuthResponse::success(const std::string& token, const user::UserResponse& user) {
    return AuthResponse(true, "Authentication successful", token, user);
}

AuthResponse AuthResponse::error(const std::string& message) {
    return AuthResponse(false, message);
}

Json::Value AuthResponse::toJson() const {
    Json::Value json;
    json["success"] = is_success;
    json["message"] = message;
    
    if (is_success && !token.empty()) {
        json["token"] = token;
        json["user"] = user.toJson();
    }
    
    return json;
}

} // namespace auth
} // namespace dto