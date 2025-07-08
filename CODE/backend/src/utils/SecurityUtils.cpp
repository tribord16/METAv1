// src/utils/SecurityUtils.cpp
#include "utils/SecurityUtils.h"
#include "utils/Logger.h"
#include <random>
#include <algorithm>
#include <cctype>

namespace utils {

// Patterns malveillants à détecter
const std::vector<std::string> SecurityUtils::MALICIOUS_PATTERNS = {
    "<script", "javascript:", "onload=", "onerror=", "onclick=",
    "DROP TABLE", "DELETE FROM", "INSERT INTO", "UPDATE SET",
    "UNION SELECT", "OR 1=1", "' OR '", "\" OR \"",
    "../", "..\\", "/etc/passwd", "cmd.exe"
};

const std::regex SecurityUtils::EMAIL_REGEX(
    R"(^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$)"
);

const std::regex SecurityUtils::USERNAME_REGEX(
    R"(^[a-zA-Z0-9_-]{3,50}$)"
);

bool SecurityUtils::isValidUsername(const std::string& username) {
    if (username.empty() || username.length() < 3 || username.length() > 50) {
        return false;
    }
    
    // Vérifier le pattern autorisé
    if (!std::regex_match(username, USERNAME_REGEX)) {
        return false;
    }
    
    // Vérifier qu'il ne contient pas de patterns malveillants
    if (containsMaliciousPatterns(username)) {
        return false;
    }
    
    // Vérifier qu'il ne commence/finit pas par des caractères spéciaux
    if (username.front() == '_' || username.front() == '-' ||
        username.back() == '_' || username.back() == '-') {
        return false;
    }
    
    return true;
}

bool SecurityUtils::isValidEmail(const std::string& email) {
    if (email.empty() || email.length() > 255) {
        return false;
    }
    
    return std::regex_match(email, EMAIL_REGEX) && !containsMaliciousPatterns(email);
}

bool SecurityUtils::isValidPassword(const std::string& password) {
    if (password.length() < 6 || password.length() > 128) {
        return false;
    }
    
    // Vérifications de sécurité basiques
    bool hasLetter = false;
    bool hasDigit = false;
    
    for (char c : password) {
        if (std::isalpha(c)) hasLetter = true;
        if (std::isdigit(c)) hasDigit = true;
        
        // Caractères de contrôle interdits
        if (std::iscntrl(c)) {
            return false;
        }
    }
    
    // Pour le MVP, on demande juste lettres + chiffres
    // Plus tard : majuscule + caractères spéciaux
    return hasLetter && hasDigit && !containsMaliciousPatterns(password);
}

bool SecurityUtils::containsMaliciousPatterns(const std::string& input) {
    std::string lowerInput = input;
    std::transform(lowerInput.begin(), lowerInput.end(), lowerInput.begin(), ::tolower);
    
    for (const auto& pattern : MALICIOUS_PATTERNS) {
        if (lowerInput.find(pattern) != std::string::npos) {
            Logger::warn("[Security] Malicious pattern detected: " + pattern + " in input");
            return true;
        }
    }
    
    return false;
}

std::string SecurityUtils::sanitizeString(const std::string& input) {
    std::string result = input;
    
    // Supprimer les caractères de contrôle
    result.erase(std::remove_if(result.begin(), result.end(), 
                 [](char c) { return std::iscntrl(c) && c != '\n' && c != '\t'; }), 
                 result.end());
    
    // Limiter la longueur
    if (result.length() > 1000) {
        result = result.substr(0, 1000);
    }
    
    return result;
}

std::string SecurityUtils::escapeHtml(const std::string& input) {
    std::string result = input;
    
    // Remplacements HTML
    std::vector<std::pair<std::string, std::string>> replacements = {
        {"<", "&lt;"},
        {">", "&gt;"},
        {"&", "&amp;"},
        {"\"", "&quot;"},
        {"'", "&#x27;"}
    };
    
    for (const auto& replacement : replacements) {
        size_t pos = 0;
        while ((pos = result.find(replacement.first, pos)) != std::string::npos) {
            result.replace(pos, replacement.first.length(), replacement.second);
            pos += replacement.second.length();
        }
    }
    
    return result;
}

std::string SecurityUtils::generateSecureToken(size_t length) {
    const std::string charset = 
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    
    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<> distribution(0, charset.size() - 1);
    
    std::string token;
    token.reserve(length);
    
    for (size_t i = 0; i < length; ++i) {
        token += charset[distribution(generator)];
    }
    
    return token;
}

std::string SecurityUtils::generateSessionId() {
    return generateSecureToken(32);
}

void SecurityUtils::addSecurityHeaders(drogon::HttpResponsePtr& response) {
    // Headers de sécurité recommandés
    response->addHeader("X-Content-Type-Options", "nosniff");
    response->addHeader("X-Frame-Options", "DENY");
    response->addHeader("X-XSS-Protection", "1; mode=block");
    response->addHeader("Referrer-Policy", "strict-origin-when-cross-origin");
    response->addHeader("Content-Security-Policy", 
                       "default-src 'self'; script-src 'self'; style-src 'self' 'unsafe-inline'");
    
    // Cache control pour les données sensibles
    response->addHeader("Cache-Control", "no-store, no-cache, must-revalidate, private");
    response->addHeader("Pragma", "no-cache");
}

bool SecurityUtils::isValidTeamName(const std::string& name) {
    if (name.empty() || name.length() < 2 || name.length() > 100) {
        return false;
    }
    
    // Autoriser lettres, chiffres, espaces et quelques caractères spéciaux
    std::regex teamNameRegex(R"(^[a-zA-Z0-9\s\-_.']{2,100}$)");
    if (!std::regex_match(name, teamNameRegex)) {
        return false;
    }
    
    return !containsMaliciousPatterns(name);
}

bool SecurityUtils::isValidPlayerName(const std::string& name) {
    return isValidTeamName(name); // Même règles pour l'instant
}

bool SecurityUtils::isValidDescription(const std::string& description) {
    if (description.length() > 1000) {
        return false;
    }
    
    return !containsMaliciousPatterns(description);
}

} // namespace utils