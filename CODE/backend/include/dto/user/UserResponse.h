// ====== ./include/dto/user/UserResponse.h ======
/**
 * @file UserResponse.h
 * @brief DTO de réponse utilisateur (SANS données sensibles)
 * 
 * Version "publique" des données utilisateur, sans password_hash
 * ni autres informations sensibles. Utilisé dans toutes les
 * réponses API où des données utilisateur sont retournées.
 */

#pragma once
#include "models/User.h"
#include <json/json.h>
#include <string>

namespace dto {
namespace user {

/**
 * @class UserResponse
 * @brief DTO pour exposer les données utilisateur en toute sécurité
 * 
 * Contient UNIQUEMENT les données non sensibles d'un utilisateur :
 * - Informations publiques (id, username, email)
 * - Statuts (is_active)
 * - Métadonnées (created_at)
 * 
 * SÉCURITÉ : JAMAIS de password_hash ou autres données sensibles
 */
class UserResponse {
public:
    int id;                  ///< Identifiant unique
    std::string username;    ///< Nom d'utilisateur public
    std::string email;       ///< Adresse email
    bool is_active;          ///< Statut actif/inactif
    std::string created_at;  ///< Date de création (format string)

    UserResponse() = default;
    explicit UserResponse(const models::User& user);

    /**
     * @brief Factory depuis entité User
     * @param user Entité User complète (avec données sensibles)
     * @return UserResponse sans données sensibles
     */
    static UserResponse fromUser(const models::User& user);

    /**
     * @brief Sérialisation JSON
     * @return Json::Value pour réponse HTTP
     */
    Json::Value toJson() const;
};

} // namespace user
} // namespace dto
