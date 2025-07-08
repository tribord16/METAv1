// ====== ./include/dto/auth/RegisterRequest.h ======
/**
 * @file RegisterRequest.h
 * @brief DTO de requête d'inscription utilisateur - Contrat d'API
 * @author Meta League Backend Team
 * @date 2025
 * @version 1.0
 *
 * RESPONSABILITÉS :
 * - Définir le contrat JSON pour l'inscription utilisateur
 * - Valider toutes les données d'entrée (format, longueur, caractères)
 * - Fournir des messages d'erreur clairs et localisés
 * - Sérialiser/désérialiser depuis/vers JSON
 * - Encapsuler toute la logique de validation métier
 *
 * PATTERN DTO (Data Transfer Object) :
 * - Séparation claire entre API contract et modèle métier
 * - Validation côté API (format, structure)
 * - Transformation vers entités métier
 * - Stabilité de l'API (versioning possible)
 *
 * VALIDATION IMPLÉMENTÉE :
 * - Username : 3-50 caractères, lettres/chiffres/underscore uniquement
 * - Email : format RFC compliant (regex validation)
 * - Password : minimum 6 caractères (à renforcer selon besoins)
 *
 * EXEMPLE JSON :
 * ```json
 * {
 *   "username": "alice_martin",
 *   "email": "alice@example.com", 
 *   "password": "monMotDePasseSecurise123"
 * }
 * ```
 *
 * RÉPONSE D'ERREUR :
 * ```json
 * {
 *   "success": false,
 *   "message": "Validation failed",
 *   "errors": [
 *     "Username must be 3-50 characters and contain only letters, numbers, and underscores",
 *     "Invalid email format"
 *   ]
 * }
 * ```
 */

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