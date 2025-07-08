// ====== ./include/dto/auth/AuthResponse.h ======
/**
 * @file AuthResponse.h
 * @brief DTO de réponse d'authentification unifié
 * 
 * Utilisé pour TOUTES les réponses d'authentification :
 * - Succès d'inscription (avec token + user data)
 * - Succès de connexion (avec token + user data)
 * - Échecs d'authentification (avec message d'erreur)
 * 
 * Pattern unifié pour cohérence API.
 */

#pragma once
#include "dto/user/UserResponse.h"
#include <json/json.h>
#include <string>

namespace dto {
namespace auth {

/**
 * @class AuthResponse
 * @brief DTO de réponse unifié pour toutes les opérations d'authentification
 * 
 * STRUCTURE CONDITIONNELLE :
 * - Si succès : token + user data + message positif
 * - Si échec : message d'erreur seulement
 * 
 * EXEMPLES :
 * Succès : {"success": true, "token": "...", "user": {...}}
 * Échec : {"success": false, "message": "Invalid credentials"}
 */
class AuthResponse {
public:
    bool is_success;         ///< Indicateur de succès/échec
    std::string message;     ///< Message pour l'utilisateur
    std::string token;       ///< Token JWT (si succès uniquement)
    user::UserResponse user; ///< Données utilisateur (si succès uniquement)

    AuthResponse() = default;
    AuthResponse(bool success, const std::string& message);
    AuthResponse(bool success, const std::string& message, const std::string& token, const user::UserResponse& user);

    /**
     * @brief Factory pour réponse de succès avec token
     * @param token Token JWT généré
     * @param user Données utilisateur (sans infos sensibles)
     * @return AuthResponse de succès
     */
    static AuthResponse success(const std::string& token, const user::UserResponse& user);

    /**
     * @brief Factory pour réponse d'erreur
     * @param message Message d'erreur pour l'utilisateur
     * @return AuthResponse d'échec
     */
    static AuthResponse error(const std::string& message);

    /**
     * @brief Sérialisation vers JSON pour HTTP response
     * @return Json::Value prêt pour HttpResponse
     */
    Json::Value toJson() const;
};

} // namespace auth
} // namespace dto