/**
 * @file LoginRequest.cpp
 * @brief Implémentation LoginRequest
 */

#include "dto/auth/LoginRequest.h"
#include "utils/Logger.h"

namespace dto {
namespace auth {

LoginRequest::LoginRequest(const std::string& username, const std::string& password)
    : username(username), password(password) {}

LoginRequest LoginRequest::fromJson(const Json::Value& json) {
    LoginRequest request;
    
    Logger::debug("[LoginRequest] Parsing JSON: " + json.toStyledString());
    
    // 🔧 FIX: Vérifier si les clés existent avant extraction
    if (json.isMember("username") && json["username"].isString()) {
        request.username = json["username"].asString();
    } else {
        Logger::error("[LoginRequest] Missing or invalid 'username' field");
    }
    
    if (json.isMember("password") && json["password"].isString()) {
        request.password = json["password"].asString();
    } else {
        Logger::error("[LoginRequest] Missing or invalid 'password' field");
    }
    
    Logger::debug("[LoginRequest] Final values - username: '" + request.username + 
                  "', password length: " + std::to_string(request.password.length()));
    
    return request;
}

bool LoginRequest::isValid() const {
    bool valid = !username.empty() && !password.empty();
    Logger::debug("[LoginRequest] Validation result: " + std::string(valid ? "VALID" : "INVALID") +
                  " (username empty: " + std::string(username.empty() ? "yes" : "no") +
                  ", password empty: " + std::string(password.empty() ? "yes" : "no") + ")");
    return valid;
}

std::vector<std::string> LoginRequest::getErrors() const {
    std::vector<std::string> errors;
    
    if (username.empty()) {
        errors.push_back("Username is required");
        Logger::debug("[LoginRequest] Username is empty");
    }
    
    if (password.empty()) {
        errors.push_back("Password is required");
        Logger::debug("[LoginRequest] Password is empty");
    }
    
    return errors;
}

} // namespace auth
} // namespace dto