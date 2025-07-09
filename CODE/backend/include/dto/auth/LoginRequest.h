// ============================================================================
// include/dto/auth/LoginRequest.h - Login Request DTO
// ============================================================================

#pragma once
#include <nlohmann/json.hpp>
#include <string>
#include <vector>
#include "utils/JsonUtils.h" // Correction: Ajout de l'inclusion pour JsonUtilsHelper (nouveau nom)

namespace dto {
namespace auth {

struct LoginRequest {
    std::string username;
    std::string password;

    static LoginRequest fromJson(const nlohmann::json& json) {
        // Correction : utilisation du nouveau nom utils_json::JsonUtilsHelper:: pour éviter les conflits de portée
        LoginRequest request;
        request.username = utils_json::JsonUtilsHelper::getString(json, "username");
        request.password = utils_json::JsonUtilsHelper::getString(json, "password");
        return request;
    }

    std::vector<std::string> validate() const {
        std::vector<std::string> errors;
        
        if (username.empty()) {
            errors.push_back("Username is required");
        } else if (username.length() < 3) {
            errors.push_back("Username must be at least 3 characters long");
        }
        
        if (password.empty()) {
            errors.push_back("Password is required");
        } else if (password.length() < 6) {
            errors.push_back("Password must be at least 6 characters long");
        }
        
        return errors;
    }

    bool isValid() const {
        return validate().empty();
    }
};

} // namespace auth
} // namespace dto