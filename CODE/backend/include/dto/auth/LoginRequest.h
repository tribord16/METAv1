
/************************************************************
 * @file LoginRequest.h
 * @brief DTO de requête de connexion utilisateur
 *
 * Rôle :
 *   - Désérialiser les données de connexion depuis le JSON
 *   - Valider la présence des champs requis (username, password)
 *   - Fournir des messages d'erreur détaillés pour l'API
 *
 * Place dans l'architecture :
 *   - Utilisé par AuthController pour parser et valider les requêtes de login
 *
 * Dépendances :
 *   - json/json.h (sérialisation JSON)
 *   - utils/Logger (logs)
 *
 * TODO :
 *   - Ajouter la validation de la force du mot de passe côté client
 *   - Ajouter des tests unitaires sur la désérialisation et la validation
 ************************************************************/

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