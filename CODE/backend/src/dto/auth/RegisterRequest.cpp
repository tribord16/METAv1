
/************************************************************
 * @file RegisterRequest.cpp
 * @brief Implémentation du DTO RegisterRequest avec validation complète
 *
 * Rôle :
 *   - Désérialiser les données d'inscription depuis le JSON
 *   - Valider la présence et le format des champs requis (username, email, password)
 *   - Fournir des messages d'erreur détaillés pour l'API
 *
 * Place dans l'architecture :
 *   - Utilisé par AuthController pour parser et valider les requêtes d'inscription
 *
 * Dépendances :
 *   - json/json.h (sérialisation JSON)
 *   - utils/Logger (logs)
 *   - regex (validation email)
 *
 * TODO :
 *   - Ajouter la validation de la force du mot de passe (majuscule, chiffre, etc.)
 *   - Ajouter des tests unitaires sur la désérialisation et la validation
 ************************************************************/

#include "dto/auth/RegisterRequest.h"
#include <regex>
#include "utils/Logger.h"

namespace dto {
namespace auth {

RegisterRequest::RegisterRequest(const std::string& username, const std::string& email, const std::string& password)
    : username(username), email(email), password(password) {}

RegisterRequest RegisterRequest::fromJson(const Json::Value& json) {
    RegisterRequest request;
    
    Logger::debug("[RegisterRequest] Parsing JSON: " + json.toStyledString());
    
    // 🔧 FIX: Vérifier si les clés existent avant extraction
    if (json.isMember("username") && json["username"].isString()) {
        request.username = json["username"].asString();
    } else {
        Logger::error("[RegisterRequest] Missing or invalid 'username' field");
    }
    
    if (json.isMember("email") && json["email"].isString()) {
        request.email = json["email"].asString();
    } else {
        Logger::error("[RegisterRequest] Missing or invalid 'email' field");
    }
    
    if (json.isMember("password") && json["password"].isString()) {
        request.password = json["password"].asString();
    } else {
        Logger::error("[RegisterRequest] Missing or invalid 'password' field");
    }
    
    Logger::debug("[RegisterRequest] Final values - username: '" + request.username + 
                  "', email: '" + request.email + 
                  "', password length: " + std::to_string(request.password.length()));
    
    return request;
}

// Reste du code inchangé...
bool RegisterRequest::isValid() const {
    bool valid = isValidUsername(username) && isValidEmail(email) && isValidPassword(password);
    
    Logger::debug("[RegisterRequest] Validation - username valid: " + std::string(isValidUsername(username) ? "true" : "false") +
                  ", email valid: " + std::string(isValidEmail(email) ? "true" : "false") +
                  ", password valid: " + std::string(isValidPassword(password) ? "true" : "false"));
    
    return valid;
}

std::vector<std::string> RegisterRequest::getErrors() const {
    std::vector<std::string> errors;
    
    if (!isValidUsername(username)) {
        errors.push_back("Username must be 3-50 characters and contain only letters, numbers, and underscores");
        Logger::debug("[RegisterRequest] Username validation failed for: '" + username + "' (length: " + std::to_string(username.length()) + ")");
    }
    
    if (!isValidEmail(email)) {
        errors.push_back("Invalid email format");
        Logger::debug("[RegisterRequest] Email validation failed for: '" + email + "'");
    }
    
    if (!isValidPassword(password)) {
        errors.push_back("Password must be at least 6 characters long");
        Logger::debug("[RegisterRequest] Password validation failed - length: " + std::to_string(password.length()));
    }
    
    return errors;
}

bool RegisterRequest::isValidEmail(const std::string& email) const {
    if (email.empty()) return false;
    
    const std::regex email_regex(R"(^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$)");
    return std::regex_match(email, email_regex);
}

bool RegisterRequest::isValidUsername(const std::string& username) const {
    if (username.length() < 3 || username.length() > 50) return false;
    
    const std::regex username_regex(R"(^[a-zA-Z0-9_]+$)");
    return std::regex_match(username, username_regex);
}

bool RegisterRequest::isValidPassword(const std::string& password) const {
    return password.length() >= 6;
}

} // namespace auth
} // namespace dto