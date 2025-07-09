
/************************************************************
 * @file ApiResponse.h
 * @brief DTO de réponse API générique pour toutes les opérations
 *
 * Rôle :
 *   - Fournir une structure de réponse uniforme pour toutes les routes API
 *   - Faciliter la sérialisation JSON des réponses (succès/erreur)
 *
 * Place dans l'architecture :
 *   - Utilisé par tous les contrôleurs/services pour retourner des réponses HTTP
 *
 * Dépendances :
 *   - json/json.h (sérialisation JSON)
 *   - string, vector
 *
 * TODO :
 *   - Ajouter des champs pour pagination, meta, etc. si besoin
 *   - Ajouter des tests unitaires sur la sérialisation
 ************************************************************/

#pragma once
#include <json/json.h>
#include <string>
#include <vector>

namespace dto {
namespace common {

/**
 * @class ApiResponse
 * @brief DTO de réponse standard pour toutes les opérations API
 * 
 * STRUCTURE UNIFORME :
 * ```json
 * {
 *   "success": true/false,
 *   "message": "Description de l'opération",
 *   "data": { ... },           // Si succès avec données
 *   "errors": ["...", "..."]   // Si échec avec détails
 * }
 * ```
 * 
 * UTILISATION :
 * - Succès simple : ApiResponse::success("Operation completed")
 * - Succès avec data : ApiResponse::success("Data retrieved", jsonData)
 * - Erreur simple : ApiResponse::error("Something went wrong")
 * - Erreur avec détails : ApiResponse::error("Validation failed", errorsList)
 */
class ApiResponse {
public:
    bool is_success;                    ///< Indicateur succès/échec
    std::string message;                ///< Message principal
    Json::Value data;                   ///< Données métier (si succès)
    std::vector<std::string> errors;    ///< Détails d'erreurs (si échec)

    ApiResponse() = default;
    ApiResponse(bool success, const std::string& message);
    ApiResponse(bool success, const std::string& message, const Json::Value& data);

    // Factory methods pour clarté du code
    static ApiResponse success(const std::string& message = "Success");
    static ApiResponse success(const std::string& message, const Json::Value& data);
    static ApiResponse error(const std::string& message);
    static ApiResponse error(const std::string& message, const std::vector<std::string>& errors);

    /**
     * @brief Sérialisation complète vers JSON
     * @return Json::Value avec tous les champs appropriés
     */
    Json::Value toJson() const;
};

} // namespace common
} // namespace dto