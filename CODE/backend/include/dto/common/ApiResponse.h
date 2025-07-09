// ============================================================================
// include/dto/common/ApiResponse.h - Standardized API Response
// ============================================================================

#pragma once
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

namespace dto {
namespace common {

template<typename T>
class ApiResponse {
public:
    bool success;
    std::string message;
    T data;
    std::vector<std::string> errors;
    long timestamp;

    ApiResponse(bool success, const std::string& message, const T& data = T{}, 
                const std::vector<std::string>& errors = {})
        : success(success), message(message), data(data), errors(errors) {
        timestamp = std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
    }

    static ApiResponse<T> createSuccess(const std::string& message, const T& data = T{}) {
        return ApiResponse<T>(true, message, data);
    }

    static ApiResponse<T> createError(const std::string& message, 
                                     const std::vector<std::string>& errors = {}) {
        return ApiResponse<T>(false, message, T{}, errors);
    }

    nlohmann::json toJson() const {
        nlohmann::json json;
        json["success"] = success;
        json["message"] = message;
        json["timestamp"] = timestamp;
        
        if constexpr (std::is_same_v<T, nlohmann::json>) {
            if (!data.empty()) {
                json["data"] = data;
            }
        } else if constexpr (std::is_void_v<T>) {
            // No data field for void responses
        } else {
            json["data"] = data;
        }
        
        if (!errors.empty()) {
            json["errors"] = errors;
        }
        
        return json;
    }
};

// Specialized for void data (no data field)
using VoidResponse = ApiResponse<std::nullptr_t>;

} // namespace common
} // namespace dto