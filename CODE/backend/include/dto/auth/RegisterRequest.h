
/************************************************************
 * @file RegisterRequest.h
 * @brief DTO de requête d'inscription utilisateur - Contrat d'API
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

#pragma once
#include <json/json.h>
#include <string>
#include <vector>

namespace dto {
namespace auth {

/**
 * @class RegisterRequest
 * @brief DTO pour les données d'inscription d'un nouvel utilisateur
 *
 * Cette classe encapsule toutes les données nécessaires à l'inscription
 * d'un nouvel utilisateur et fournit une validation complète selon
 * les règles métier de l'application.
 *
 * CYCLE DE VIE TYPIQUE :
 * 1. Réception JSON HTTP → fromJson() → RegisterRequest
 * 2. Validation → isValid() + getErrors()
 * 3. Si valide → transformation vers User model
 * 4. Si invalide → retour erreurs au client
 *
 * RÈGLES DE VALIDATION :
 * - Username : unique, 3-50 chars, alphanumeric + underscore
 * - Email : unique, format RFC valide
 * - Password : minimum 6 caractères (extensible pour complexité)
 *
 * SÉCURITÉ :
 * - Validation côté serveur OBLIGATOIRE (jamais faire confiance au client)
 * - Pas de stockage du password en clair (hash immédiat)
 * - Messages d'erreur informatifs mais pas révélateurs
 */
class RegisterRequest {
public:
    std::string username;    ///< Nom d'utilisateur unique (3-50 chars)
    std::string email;       ///< Adresse email unique et valide
    std::string password;    ///< Mot de passe en clair (sera hashé immédiatement)

    /**
     * @brief Constructeur par défaut
     * Initialise un DTO vide pour parsing JSON
     */
    RegisterRequest() = default;

    /**
     * @brief Constructeur avec paramètres
     * @param username Nom d'utilisateur
     * @param email Adresse email
     * @param password Mot de passe en clair
     */
    RegisterRequest(const std::string& username, const std::string& email, const std::string& password);

    /**
     * @brief Factory method : création depuis JSON
     * 
     * Parse un objet JSON reçu via HTTP POST en DTO typé.
     * Gère les champs manquants (valeurs par défaut vides).
     * 
     * @param json Objet JSON parsé depuis le body HTTP
     * @return RegisterRequest DTO hydraté (à valider ensuite)
     * 
     * EXEMPLE :
     * ```cpp
     * // JSON reçu :
     * // {"username": "alice", "email": "alice@ex.com", "password": "secret"}
     * 
     * auto jsonObj = req->getJsonObject();
     * auto registerReq = RegisterRequest::fromJson(*jsonObj);
     * 
     * if (registerReq.isValid()) {
     *     // Procéder à l'inscription
     * }
     * ```
     */
    static RegisterRequest fromJson(const Json::Value& json);

    /**
     * @brief Valide TOUTES les données du DTO
     * 
     * Effectue une validation complète selon les règles métier :
     * - Format et longueur username
     * - Format email (regex RFC)
     * - Longueur minimale password
     * 
     * @return true si toutes les validations passent, false sinon
     * 
     * UTILISATION :
     * ```cpp
     * auto req = RegisterRequest::fromJson(json);
     * if (!req.isValid()) {
     *     auto errors = req.getErrors();
     *     return createValidationErrorResponse(errors);
     * }
     * ```
     */
    bool isValid() const;

    /**
     * @brief Retourne la liste détaillée des erreurs de validation
     * 
     * Génère des messages d'erreur clairs et actionnables pour l'utilisateur.
     * Utile pour affichage dans l'UI côté client.
     * 
     * @return std::vector<std::string> Liste des messages d'erreur
     * 
     * EXEMPLES DE MESSAGES :
     * - "Username must be 3-50 characters and contain only letters, numbers, and underscores"
     * - "Invalid email format"
     * - "Password must be at least 6 characters long"
     */
    std::vector<std::string> getErrors() const;

private:
    /**
     * @brief Valide le format de l'adresse email
     * @param email Email à valider
     * @return true si format valide selon regex RFC
     */
    bool isValidEmail(const std::string& email) const;

    /**
     * @brief Valide le format du nom d'utilisateur
     * @param username Username à valider
     * @return true si respecte les règles (3-50 chars, alphanumeric + _)
     */
    bool isValidUsername(const std::string& username) const;

    /**
     * @brief Valide la complexité du mot de passe
     * @param password Password à valider
     * @return true si respecte les règles (min 6 chars actuellement)
     */
    bool isValidPassword(const std::string& password) const;
};

} // namespace auth
} // namespace dto