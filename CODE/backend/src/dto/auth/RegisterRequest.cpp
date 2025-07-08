// ====== ./src/dto/auth/RegisterRequest.cpp ======
/**
 * @file RegisterRequest.cpp
 * @brief Implémentation du DTO RegisterRequest avec validation complète
 */

#include "dto/auth/RegisterRequest.h"
#include <regex>

namespace dto {
namespace auth {

RegisterRequest::RegisterRequest(const std::string& username, const std::string& email, const std::string& password)
    : username(username), email(email), password(password) {}

RegisterRequest RegisterRequest::fromJson(const Json::Value& json) {
    RegisterRequest request;
    
    // Extraction avec valeurs par défaut (chaînes vides si champ absent)
    request.username = json.get("username", "").asString();
    request.email = json.get("email", "").asString();
    request.password = json.get("password", "").asString();
    
    return request;
}

bool RegisterRequest::isValid() const {
    return isValidUsername(username) && isValidEmail(email) && isValidPassword(password);
}

std::vector<std::string> RegisterRequest::getErrors() const {
    std::vector<std::string> errors;
    
    if (!isValidUsername(username)) {
        errors.push_back("Username must be 3-50 characters and contain only letters, numbers, and underscores");
    }
    
    if (!isValidEmail(email)) {
        errors.push_back("Invalid email format");
    }
    
    if (!isValidPassword(password)) {
        errors.push_back("Password must be at least 6 characters long");
    }
    
    return errors;
}

bool RegisterRequest::isValidEmail(const std::string& email) const {
    // Regex RFC 5322 simplifiée mais robuste
    const std::regex email_regex(R"(^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$)");
    return std::regex_match(email, email_regex);
}

bool RegisterRequest::isValidUsername(const std::string& username) const {
    // Vérification longueur
    if (username.length() < 3 || username.length() > 50) return false;
    
    // Vérification caractères autorisés : lettres, chiffres, underscore
    const std::regex username_regex(R"(^[a-zA-Z0-9_]+$)");
    return std::regex_match(username, username_regex);
}

bool RegisterRequest::isValidPassword(const std::string& password) const {
    // Validation minimale : au moins 6 caractères
    // TODO: renforcer selon besoins (majuscules, chiffres, caractères spéciaux)
    return password.length() >= 6;
}

} // namespace auth
} // namespace dto