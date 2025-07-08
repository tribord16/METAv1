/**
 * @file ApiResponse.cpp
 * @brief Implémentation ApiResponse
 */

#include "dto/common/ApiResponse.h"

namespace dto {
namespace common {

ApiResponse::ApiResponse(bool success, const std::string& message)
    : is_success(success), message(message) {}

ApiResponse::ApiResponse(bool success, const std::string& message, const Json::Value& data)
    : is_success(success), message(message), data(data) {}

ApiResponse ApiResponse::success(const std::string& message) {
    return ApiResponse(true, message);
}

ApiResponse ApiResponse::success(const std::string& message, const Json::Value& data) {
    return ApiResponse(true, message, data);
}

ApiResponse ApiResponse::error(const std::string& message) {
    return ApiResponse(false, message);
}

ApiResponse ApiResponse::error(const std::string& message, const std::vector<std::string>& errors) {
    ApiResponse response(false, message);
    response.errors = errors;
    return response;
}

Json::Value ApiResponse::toJson() const {
    Json::Value json;
    json["success"] = is_success;
    json["message"] = message;
    
    if (!data.isNull()) {
        json["data"] = data;
    }
    
    if (!errors.empty()) {
        Json::Value errorsArray(Json::arrayValue);
        for (const auto& error : errors) {
            errorsArray.append(error);
        }
        json["errors"] = errorsArray;
    }
    
    return json;
}

} // namespace common
} // namespace dto