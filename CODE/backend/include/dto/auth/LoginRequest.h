// ====== ./include/dto/auth/LoginRequest.h ======
/**
 * @file LoginRequest.h
 * @brief DTO de requête de connexion utilisateur
 * 
 * DTO simple pour l'authentification par username/password.
 * Validation minimale (champs requis) car la validation métier
 * se fait côté service (vérification credentials en base).
 */

#pragma once
#include <json/json.h>
#include <string>
#include <vector>

namespace dto {
namespace auth {

/**
 * @class LoginRequest
 * @brief DTO pour les données de connexion utilisateur
 * 
 * Structure simple contenant les credentials d'authentification.
 * Validation minimale car la vraie validation se fait en comparant
 * avec les données stockées en base.
 */
class LoginRequest {
public:
    std::string username;    ///< Username ou email selon configuration
    std::string password;    ///< Mot de passe en clair

    LoginRequest() = default;
    LoginRequest(const std::string& username, const std::string& password);

    /**
     * @brief Factory method depuis JSON
     * @param json Objet JSON {"username": "...", "password": "..."}
     * @return LoginRequest DTO hydraté
     */
    static LoginRequest fromJson(const Json::Value& json);

    /**
     * @brief Validation basique : champs non vides
     * @return true si username ET password non vides
     */
    bool isValid() const;

    /**
     * @brief Messages d'erreur si validation échoue
     * @return Liste des champs manquants
     */
    std::vector<std::string> getErrors() const;
};

} // namespace auth
} // namespace dto