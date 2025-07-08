/**
 * @file LoginRequest.cpp
 * @brief Implémentation LoginRequest
 */

#include "dto/auth/LoginRequest.h"

namespace dto {
namespace auth {

LoginRequest::LoginRequest(const std::string& username, const std::string& password)
    : username(username), password(password) {}

LoginRequest LoginRequest::fromJson(const Json::Value& json) {
    LoginRequest request;
    request.username = json.get("username", "").asString();
    request.password = json.get("password", "").asString();
    return request;
}

bool LoginRequest::isValid() const {
    return !username.empty() && !password.empty();
}

std::vector<std::string> LoginRequest::getErrors() const {
    std::vector<std::string> errors;
    
    if (username.empty()) {
        errors.push_back("Username is required");
    }
    
    if (password.empty()) {
        errors.push_back("Password is required");
    }
    
    return errors;
}

} // namespace auth
} // namespace dto