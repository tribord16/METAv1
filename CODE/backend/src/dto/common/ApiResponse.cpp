
/************************************************************
 * @file ApiResponse.cpp
 * @brief Implémentation du DTO de réponse API générique
 *
 * Rôle :
 *   - Fournir une structure de réponse uniforme pour toutes les routes API
 *   - Faciliter la sérialisation JSON des réponses (succès/erreur)
 *
 * Place dans l'architecture :
 *   - Utilisé par tous les contrôleurs/services pour retourner des réponses HTTP
 *
 * Dépendances :
 *   - json/json.h (sérialisation JSON)
 *   - string, vector
 *
 * TODO :
 *   - Ajouter des champs pour pagination, meta, etc. si besoin
 *   - Ajouter des tests unitaires sur la sérialisation
 ************************************************************/

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