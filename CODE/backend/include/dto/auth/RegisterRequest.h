// ============================================================================
// include/dto/auth/RegisterRequest.h - Registration Request DTO
// ============================================================================

#pragma once
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

#include <regex>
#include "utils/JsonUtils.h" // Correction: Ajout de l'inclusion pour JsonUtilsHelper (nouveau nom)

namespace dto {
namespace auth {

struct RegisterRequest {
    std::string username;
    std::string email;
    std::string password;

    static RegisterRequest fromJson(const nlohmann::json& json) {
        // Correction : utilisation du nouveau nom utils_json::JsonUtilsHelper:: pour éviter les conflits de portée
        RegisterRequest request;
        request.username = utils_json::JsonUtilsHelper::getString(json, "username");
        request.email = utils_json::JsonUtilsHelper::getString(json, "email");
        request.password = utils_json::JsonUtilsHelper::getString(json, "password");
        return request;
    }

    std::vector<std::string> validate() const {
        std::vector<std::string> errors;
        
        // Username validation
        if (username.empty()) {
            errors.push_back("Username is required");
        } else if (username.length() < 3 || username.length() > 50) {
            errors.push_back("Username must be between 3 and 50 characters");
        } else if (!std::regex_match(username, std::regex("^[a-zA-Z0-9_]+$"))) {
            errors.push_back("Username can only contain letters, numbers, and underscores");
        }
        
        // Email validation
        if (email.empty()) {
            errors.push_back("Email is required");
        } else if (!isValidEmail(email)) {
            errors.push_back("Invalid email format");
        }
        
        // Password validation
        if (password.empty()) {
            errors.push_back("Password is required");
        } else if (password.length() < 6) {
            errors.push_back("Password must be at least 6 characters long");
        } else if (password.length() > 128) {
            errors.push_back("Password must be less than 128 characters");
        }
        
        return errors;
    }

    bool isValid() const {
        return validate().empty();
    }

private:
    bool isValidEmail(const std::string& email) const {
        const std::regex emailRegex(R"(^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$)");
        return std::regex_match(email, emailRegex);
    }
};

} // namespace auth
} // namespace dto