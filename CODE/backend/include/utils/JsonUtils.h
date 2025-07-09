// ============================================================================
// include/utils/JsonUtils.h - JSON Helper Functions
// ============================================================================

#pragma once
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

// Correction : renommage du namespace pour éviter les conflits de portée
namespace utils_json {

// Correction : renommage de la classe pour éviter les conflits de portée
class JsonUtilsHelper {
public:
    // Create standardized API response
    static nlohmann::json createResponse(bool success, const std::string& message, 
                                        const nlohmann::json& data = nlohmann::json::object()) {
        nlohmann::json response;
        response["success"] = success;
        response["message"] = message;
        response["timestamp"] = std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
        
        if (!data.empty()) {
            response["data"] = data;
        }
        
        return response;
    }

    // Create error response with validation errors
    static nlohmann::json createErrorResponse(const std::string& message, 
                                             const std::vector<std::string>& errors = {}) {
        auto response = createResponse(false, message);
        if (!errors.empty()) {
            response["errors"] = errors;
        }
        return response;
    }

    // Validate required fields in JSON
    static std::vector<std::string> validateRequiredFields(const nlohmann::json& json, 
                                                          const std::vector<std::string>& requiredFields) {
        std::vector<std::string> missingFields;
        for (const auto& field : requiredFields) {
            if (!json.contains(field) || json[field].is_null()) {
                missingFields.push_back(field);
            }
        }
        return missingFields;
    }

    // Safe string extraction with default value
    static std::string getString(const nlohmann::json& json, const std::string& key, 
                                const std::string& defaultValue = "") {
        if (json.contains(key) && json[key].is_string()) {
            return json[key].get<std::string>();
        }
        return defaultValue;
    }

    // Safe integer extraction with default value
    static int getInt(const nlohmann::json& json, const std::string& key, int defaultValue = 0) {
        if (json.contains(key) && json[key].is_number_integer()) {
            return json[key].get<int>();
        }
        return defaultValue;
    }

    // Safe double extraction with default value
    static double getDouble(const nlohmann::json& json, const std::string& key, double defaultValue = 0.0) {
        if (json.contains(key) && json[key].is_number()) {
            return json[key].get<double>();
        }
        return defaultValue;
    }

    // Safe boolean extraction with default value
    static bool getBool(const nlohmann::json& json, const std::string& key, bool defaultValue = false) {
        if (json.contains(key) && json[key].is_boolean()) {
            return json[key].get<bool>();
        }
        return defaultValue;
    }
};

} // namespace utils_json