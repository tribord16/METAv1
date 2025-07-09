
/************************************************************
 * @file AuthResponse.cpp
 * @brief Implémentation du DTO de réponse d'authentification unifié
 *
 * Rôle :
 *   - Fournir une structure de réponse uniforme pour toutes les opérations d'authentification
 *   - Faciliter la sérialisation JSON des réponses (succès/erreur)
 *
 * Place dans l'architecture :
 *   - Utilisé par AuthController pour retourner les réponses d'authentification
 *
 * Dépendances :
 *   - json/json.h (sérialisation JSON)
 *   - dto/user/UserResponse (données utilisateur)
 *
 * TODO :
 *   - Ajouter des champs pour la gestion du refresh token
 *   - Ajouter des tests unitaires sur la sérialisation
 ************************************************************/

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