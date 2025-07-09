/**
 * @file CreateCorporationRequest.h
 * @brief DTO de création de corporation - contrat API
 * @author MetaLeague Backend Team
 * @date 2025
 * @version 1.0
 *
 * RESPONSABILITÉS :
 * - Définir le contrat JSON pour création corporation
 * - Valider toutes les données d'entrée
 * - Fournir messages d'erreur clairs
 * - Sérialiser/désérialiser JSON
 */

#pragma once
#include <json/json.h>
#include <string>
#include <vector>

namespace dto {
namespace corporation {

/**
 * @class CreateCorporationRequest
 * @brief DTO pour création de nouvelle corporation
 */
class CreateCorporationRequest {
public:
    std::string name;                    ///< Nom de la corporation (3-50 chars)
    double initial_budget;               ///< Budget initial demandé
    bool esports_active;                 ///< Division esports activée
    bool racing_active;                  ///< Division course activée
    bool tactical_active;                ///< Division tactique activée
    bool innovation_active;              ///< Division innovation activée

    CreateCorporationRequest() = default;
    
    /**
     * @brief Factory depuis JSON
     * @param json Objet JSON depuis HTTP request
     * @return CreateCorporationRequest DTO hydraté
     */
    static CreateCorporationRequest fromJson(const Json::Value& json);

    /**
     * @brief Validation complète des données
     * @return true si toutes validations passent
     */
    bool isValid() const;

    /**
     * @brief Messages d'erreur détaillés
     * @return vector<string> Liste des erreurs
     */
    std::vector<std::string> getErrors() const;

private:
    bool isValidName(const std::string& name) const;
    bool isValidBudget(double budget) const;
    bool hasAtLeastOneDivision() const;
};

} // namespace corporation
} // namespace dto