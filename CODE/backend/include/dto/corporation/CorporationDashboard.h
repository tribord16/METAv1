/**
 * @file CorporationDashboard.h
 * @brief DTO dashboard corporation - données écran principal
 * @author MetaLeague Backend Team
 * @date 2025
 * @version 1.0
 *
 * RESPONSABILITÉS :
 * - Agréger toutes les données dashboard
 * - Format optimisé pour UI React
 * - Calculs statistiques pré-calculés
 * - Performance optimisée (éviter N+1 queries)
 */

#pragma once
#include <models/Corporations.h>

#include <json/json.h>
#include <vector>

// Alias pour éviter d'écrire le namespace complet partout
using Corporations = drogon_model::meta_league_dev::Corporations;

namespace dto {
namespace corporation {

/**
 * @struct FinancialSummary
 * @brief Résumé financier pour dashboard
 */
struct FinancialSummary {
    double current_budget;               ///< Budget actuel
    double monthly_income;               ///< Revenus mensuels
    double monthly_expenses;             ///< Dépenses mensuelles
    double net_monthly;                  ///< Résultat mensuel
    double projected_budget_3m;          ///< Projection 3 mois
    
    Json::Value toJson() const;
};

/**
 * @struct TeamSummary
 * @brief Résumé équipe pour dashboard
 */
struct TeamSummary {
    int total_players;                   ///< Nombre total joueurs
    double average_rating;               ///< Rating moyen équipe
    double total_salary;                 ///< Masse salariale totale
    std::vector<std::string> missing_positions; ///< Positions non couvertes
    double team_chemistry;               ///< Chimie d'équipe globale
    
    Json::Value toJson() const;
};

/**
 * @struct UpcomingEvents
 * @brief Événements à venir
 */
struct UpcomingEvents {
    std::vector<std::string> next_matches;    ///< Prochains matchs
    std::vector<std::string> deadlines;       ///< Deadlines importantes
    std::vector<std::string> opportunities;   ///< Opportunités business
    
    Json::Value toJson() const;
};

/**
 * @class CorporationDashboard
 * @brief DTO complet du dashboard corporation
 */
class CorporationDashboard {
public:
    Corporations corporation;      ///< Infos corporation de base
    FinancialSummary finances;           ///< Résumé financier
    TeamSummary team;                    ///< Résumé équipe
    UpcomingEvents events;               ///< Événements à venir
    std::vector<std::string> notifications; ///< Actions requises
    int current_week;                    ///< Semaine de jeu
    std::string current_phase;           ///< Phase de saison

    /**
     * @brief Sérialise tout le dashboard vers JSON
     * @return Json::Value Dashboard complet pour frontend
     */
    Json::Value toJson() const;
};

} // namespace corporation
} // namespace dto