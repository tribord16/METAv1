// include/utils/SecurityUtils.h
/**
 * @file SecurityUtils.h
 * @brief Utilitaires de sécurité pour validation et protection
 * 
 * FONCTIONNALITÉS :
 * - Validation inputs avancée (anti-injection, XSS)
 * - Génération de tokens sécurisés
 * - Headers de sécurité HTTP
 * - Détection de patterns malveillants
 */

#pragma once
#include <string>
#include <vector>
#include <regex>
#include <drogon/HttpResponse.h>

namespace utils {

class SecurityUtils {
public:
    // Validation des entrées
    static bool isValidUsername(const std::string& username);
    static bool isValidEmail(const std::string& email);
    static bool isValidPassword(const std::string& password);
    static bool containsMaliciousPatterns(const std::string& input);
    
    // Sanitization
    static std::string sanitizeString(const std::string& input);
    static std::string escapeHtml(const std::string& input);
    
    // Génération sécurisée
    static std::string generateSecureToken(size_t length = 32);
    static std::string generateSessionId();
    
    // Headers de sécurité
    static void addSecurityHeaders(drogon::HttpResponsePtr& response);
    
    // Validation spécifique au jeu
    static bool isValidTeamName(const std::string& name);
    static bool isValidPlayerName(const std::string& name);
    static bool isValidDescription(const std::string& description);
    
private:
    static const std::vector<std::string> MALICIOUS_PATTERNS;
    static const std::regex EMAIL_REGEX;
    static const std::regex USERNAME_REGEX;
};

} // namespace utils