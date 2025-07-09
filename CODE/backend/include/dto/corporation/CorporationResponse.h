
/************************************************************
 * @file CorporationResponse.h
 * @brief DTO de réponse pour les corporations
 *
 * Rôle :
 *   - Sérialiser les données de corporation pour l'API
 *   - Fournir une structure uniforme pour les réponses API
 *
 * Place dans l'architecture :
 *   - Utilisé par CorporationController pour retourner les réponses de corporation
 *
 * Dépendances :
 *   - json/json.h (sérialisation JSON)
 *   - models/Corporations (accès aux données)
 *
 * TODO :
 *   - Ajouter des champs pour la pagination ou les meta-infos
 *   - Ajouter des tests unitaires sur la sérialisation
 ************************************************************/

#pragma once
#include <json/json.h>
#include <string>
#include <ctime>
#include <models/Corporations.h>

// Alias pour éviter d'écrire le namespace complet partout
using Corporations = drogon_model::meta_league_dev::Corporations;

namespace dto {
namespace corporation {

/**
 * @class CorporationResponse
 * @brief DTO de réponse pour les corporations
 * 
 * Représente la structure de réponse pour les opérations sur les corporations.
 * Utilisé pour sérialiser les données de corporation en JSON pour l'API.
 */
class CorporationResponse {
public:
    int id;                             ///< ID unique de la corporation
    std::string name;                   ///< Nom de la corporation
    bool esports_active;                ///< Division esports activée
    bool racing_active;                 ///< Division course activée
    bool tactical_active;               ///< Division tactique activée
    bool innovation_active;             ///< Division innovation activée
    double budget;                      ///< Budget actuel de la corporation
    std::time_t created_at;             ///< Date de création (timestamp)
    std::time_t updated_at;             ///< Date de dernière mise à jour (timestamp)
    int owner_id;                       ///< ID de l'utilisateur propriétaire
    bool is_success = false;            ///< Indique le succès de l'opération
    std::string message;                ///< Message d'erreur ou de succès

    /**
     * @brief Sérialise l'objet en JSON
     * @return Json::Value représentant la corporation
     * 
     */
    Json::Value toJson() const;


    /**
     * @brief Crée une réponse de succès à partir d'un modèle de corporation
     * @param corp Modèle de corporation
     * @return CorporationResponse DTO de réponse
     */
    static CorporationResponse fromModel(const Corporations& corp);
    /**
     * @brief Crée une réponse d'erreur
     * @param msg Message d'erreur
     * @return CorporationResponse DTO de réponse avec erreur
     */
    static CorporationResponse error(const std::string& msg);
    /**
     * @brief Vérifie si la réponse est un succès
     * @return true si la réponse est un succès, false sinon
     */
};

} // namespace corporation
} // namespace dto
