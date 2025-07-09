// ============================================================================
// include/dto/corporation/CreateCorporationRequest.h - Corporation Creation DTO
// ============================================================================

#pragma once
#include <nlohmann/json.hpp>
#include <string>
#include <vector>
#include "utils/JsonUtils.h"

namespace dto {
namespace corporation {

struct CreateCorporationRequest {
    std::string name;
    double initial_budget = 1000000.0;
    bool esports_active = true;
    bool racing_active = false;
    bool tactical_active = false;
    bool innovation_active = false;

    static CreateCorporationRequest fromJson(const nlohmann::json& json) {
        CreateCorporationRequest request;
        request.name = utils_json::JsonUtilsHelper::getString(json, "name");
        request.initial_budget = utils_json::JsonUtilsHelper::getDouble(json, "initial_budget", 1000000.0);
        request.esports_active = utils_json::JsonUtilsHelper::getBool(json, "esports_active", true);
        request.racing_active = utils_json::JsonUtilsHelper::getBool(json, "racing_active", false);
        request.tactical_active = utils_json::JsonUtilsHelper::getBool(json, "tactical_active", false);
        request.innovation_active = utils_json::JsonUtilsHelper::getBool(json, "innovation_active", false);
        return request;
    }

    std::vector<std::string> validate() const {
        std::vector<std::string> errors;
        
        // Name validation
        if (name.empty()) {
            errors.push_back("Corporation name is required");
        } else if (name.length() < 3 || name.length() > 100) {
            errors.push_back("Corporation name must be between 3 and 100 characters");
        }
        
        // Budget validation
        if (initial_budget < 100000.0) {
            errors.push_back("Initial budget must be at least 100,000");
        } else if (initial_budget > 50000000.0) {
            errors.push_back("Initial budget cannot exceed 50,000,000");
        }
        
        // Division validation - at least one must be active
        if (!esports_active && !racing_active && !tactical_active && !innovation_active) {
            errors.push_back("At least one division must be active");
        }
        
        return errors;
    }

    bool isValid() const {
        return validate().empty();
    }
};

} // namespace corporation
} // namespace dto