/**
 * @file CorporationService.h
 * @brief Service corporation - logique métier management
 * @author MetaLeague Backend Team
 * @date 2025
 * @version 1.0
 *
 * RESPONSABILITÉS :
 * - Orchestrer création/gestion corporations
 * - Appliquer règles business (budgets, réputation)
 * - Gérer événements automatiques (salaires, etc.)
 * - Intégrer avec autres services (Players, Matches)
 * - Fournir données dashboard
 *
 * PATTERN SERVICE LAYER :
 * - Abstraction entre controllers et repositories
 * - Logique métier centralisée
 * - Transactions business complexes
 * - Validation des règles du jeu
 */

#pragma once
#include <models/Corporations.h>
#include "repositories/CorporationRepository.h"
#include "repositories/PlayerRepository.h"
#include "dto/corporation/CreateCorporationRequest.h"
#include "dto/corporation/CorporationDashboard.h"
#include "dto/corporation/CorporationResponse.h" // DTO de sortie principal
#include <functional>
#include <memory>

// Alias pour éviter d'écrire le namespace complet partout
using Corporations = drogon_model::meta_league_dev::Corporations;

namespace services {

/**
 * @class CorporationService
 * @brief Service principal pour gestion des corporations
 *
 * Orchestre toutes les opérations corporation :
 * - Création avec validation business
 * - Gestion financière et réputation
 * - Intégration avec système de jeu
 * - Calculs statistiques avancés
 */
/**
 * @class CorporationService
 * @brief Service principal pour la gestion des corporations (Service Layer)
 *
 * RESPONSABILITÉS :
 * - Orchestration de la création, gestion, et reporting des corporations
 * - Application des règles business (budgets, réputation, contraintes ligue)
 * - Intégration avec les autres services (Players, Matches, Events)
 * - Fourniture de DTOs propres pour l'API (jamais d'exposition directe des modèles)
 *
 * ARCHITECTURE :
 * Controller <-> CorporationService <-> Repositories <-> Database
 *                        |
 *                      DTOs
 *
 * BONNES PRATIQUES :
 * - Entrée : toujours via DTOs (CreateCorporationRequest, ...)
 * - Sortie : toujours via DTOs (CorporationResponse, CorporationDashboard, ...)
 * - Jamais d'exposition directe de Corporations
 * - Callbacks asynchrones pour compatibilité avec Drogon
 * - Thread-safe (pas d'état mutable partagé)
 */
class CorporationService {
private:
    std::shared_ptr<repositories::CorporationRepository> corpRepo_;
    //std::shared_ptr<repositories::PlayerRepository> playerRepo_;

public:
    /**
     * @brief Constructeur avec injection de dépendances
     * @param corpRepo Repository des corporations
     * @param playerRepo Repository des joueurs
     */
    /**
     * @brief Constructeur avec injection de dépendances
     * @param corpRepo Repository des corporations
     * @param playerRepo Repository des joueurs
     *
     * @note Les repositories doivent être thread-safe
     * @note Utilise shared_ptr pour la gestion mémoire
     */
    CorporationService(std::shared_ptr<repositories::CorporationRepository> corpRepo);

    /**
     * @brief Crée une nouvelle corporation avec validation complète
     * @param request DTO de création validé
     * @param userId ID de l'utilisateur propriétaire
     * @param callback Fonction appelée avec la corporation créée
     * 
     * PROCESSUS COMPLET :
     * 1. Validation business (nom unique, divisions cohérentes)
     * 2. Calcul budget initial selon choix divisions
     * 3. Création corporation en base
     * 4. Initialisation GameState associé
     * 5. Génération événements de démarrage
     * 
     * EXEMPLE :
     * ```cpp
     * CreateCorporationRequest req;
     * req.name = "Team Quantum";
     * req.esports_active = true;
     * req.initial_budget = 1000000.0;
     * 
     * corpService->createCorporation(req, userId, [](const Corporation& corp) {
     *     Logger::info("Corporation created: " + corp.name);
     *     redirectToGameDashboard(corp.id);
     * });
     * ```
     */
    /**
     * @brief Crée une nouvelle corporation avec validation complète
     * @param request DTO de création validé (CreateCorporationRequest)
     * @param userId ID de l'utilisateur propriétaire
     * @param callback Fonction appelée avec le DTO de réponse (CorporationResponse)
     *
     * PROCESSUS :
     * 1. Validation business (unicité nom, cohérence divisions)
     * 2. Calcul budget initial
     * 3. Création en base
     * 4. Génération des événements de démarrage
     *
     * @note Ne jamais exposer Corporations directement
     */
    void createCorporation(const dto::corporation::CreateCorporationRequest& request,
                          int userId,
                          std::function<void(const dto::corporation::CorporationResponse&)> callback);

    void listCorporationsByUser(int userId,
                               std::function<void(const std::vector<dto::corporation::CorporationResponse>&)> callback);

    void getCorporationById(int corpId, int userId,
                            std::function<void(const dto::corporation::CorporationResponse&)> callback);

    void updateCorporation(int corpId, const dto::corporation::CreateCorporationRequest& request, int userId,
                          std::function<void(const dto::corporation::CorporationResponse&)> callback);

    void deleteCorporation(int corpId, int userId,
                          std::function<void(bool)> callback);

    void getCorporationDashboard(int corpId, int userId,
                                std::function<void(const dto::corporation::CorporationDashboard&)> callback);

    /**
     * @brief Obtient le dashboard complet d'une corporation
     * @param corpId ID de la corporation
     * @param callback Fonction appelée avec les données dashboard
     * 
     * DONNÉES DASHBOARD :
     * - Informations corporation (nom, budget, réputation)
     * - Stats joueurs (nombre, salaire total, rating moyen)
     * - Finances (revenus/dépenses du mois)
     * - Calendrier (prochains matchs, événements)
     * - Notifications (actions requises)
     * 
     * UTILISATION :
     * ```cpp
     * corpService->getDashboard(corpId, [](const CorporationDashboard& dashboard) {
     *     updateDashboardUI(dashboard);
     * });
     * ```
     */
    /**
     * @brief Récupère le dashboard complet d'une corporation (DTO)
     * @param corpId ID de la corporation
     * @param callback Fonction appelée avec le DTO dashboard
     */
    void getDashboard(int corpId,
                     std::function<void(const dto::corporation::CorporationDashboard&)> callback);

    /**
     * @brief Traite les événements automatiques hebdomadaires
     * @param corpId ID de la corporation
     * @param callback Fonction appelée avec les événements traités
     * 
     * ÉVÉNEMENTS AUTOMATIQUES :
     * - Paiement salaires (premier lundi du mois)
     * - Revenus sponsors (dates contractuelles)
     * - Maintenance facilities (coûts récurrents)
     * - Updates réputation (basés sur performances)
     * - Événements aléatoires (sponsors, scandales, etc.)
     */
    /**
     * @brief Traite les événements automatiques hebdomadaires
     * @param corpId ID de la corporation
     * @param callback Fonction appelée avec la liste des événements traités (DTO ou std::vector)
     */
    void processWeeklyEvents(int corpId,
                            std::function<void(const std::vector<std::string>&)> callback);

    /**
     * @brief Calcule les coûts mensuels d'une corporation
     * @param corpId ID de la corporation
     * @param callback Fonction appelée avec le total des coûts
     * 
     * COÛTS CALCULÉS :
     * - Salaires joueurs (total équipe)
     * - Salaires staff (coaches, analysts)
     * - Facilities (gaming house, training center)
     * - Utilities (électricité, internet, équipement)
     */
    /**
     * @brief Calcule les coûts mensuels d'une corporation
     * @param corpId ID de la corporation
     * @param callback Fonction appelée avec le total des coûts (double)
     */
    void calculateMonthlyCosts(int corpId,
                              std::function<void(double)> callback);

    /**
     * @brief Vérifie si corporation peut recruter un joueur
     * @param corpId ID de la corporation
     * @param playerId ID du joueur
     * @param proposedSalary Salaire proposé
     * @param callback Fonction appelée avec faisabilité + contraintes
     * 
     * VÉRIFICATIONS :
     * - Budget suffisant (salaire + signing bonus)
     * - Roster cap (max 10 joueurs par corporation)
     * - Position available (max 3 par rôle)
     * - Salary cap league (respect règles compétition)
     * 
     * EXEMPLE :
     * ```cpp
     * corpService->canSignPlayer(corpId, playerId, 15000, 
     *     [](bool canSign, const std::vector<std::string>& constraints) {
     *         if (canSign) {
     *             proceedWithSigning();
     *         } else {
     *             showConstraints(constraints); // "Budget insufficient", etc.
     *         }
     *     }
     * );
     * ```
     */
    /**
     * @brief Vérifie si une corporation peut recruter un joueur
     * @param corpId ID de la corporation
     * @param playerId ID du joueur
     * @param proposedSalary Salaire proposé
     * @param callback Fonction appelée avec faisabilité + contraintes (DTO ou bool + vector)
     */
    void canSignPlayer(int corpId, int playerId, double proposedSalary,
                      std::function<void(bool, const std::vector<std::string>&)> callback);

    /**
     * @brief Met à jour la réputation basée sur performance
     * @param corpId ID de la corporation
     * @param performanceData Données de performance récentes
     * @param callback Fonction appelée avec nouvelle réputation
     * 
     * FACTEURS RÉPUTATION :
     * - Résultats compétitions (+/-5 à +/-15 selon importance)
     * - Comportement joueurs (+/-2 selon incidents)
     * - Relations médias (+/-3 selon PR management)
     * - Innovation/achievements (+5 pour premières)
     */
    /**
     * @brief Met à jour la réputation d'une corporation selon les performances
     * @param corpId ID de la corporation
     * @param performanceData Données de performance (JSON ou DTO)
     * @param callback Fonction appelée avec la nouvelle réputation (int)
     */
    void updateReputation(int corpId, const Json::Value& performanceData,
                         std::function<void(int)> callback);

private:
    /**
     * @brief Valide les données de création corporation
     * @param request Données à valider
     * @return vector<string> Liste des erreurs (vide si OK)
     */
    /**
     * @brief Valide les données de création de corporation (DTO)
     * @param request DTO à valider
     * @return vector<string> Liste des erreurs (vide si OK)
     */
    std::vector<std::string> validateCreationRequest(
        const dto::corporation::CreateCorporationRequest& request);

    /**
     * @brief Calcule le budget initial selon les divisions choisies
     * @param divisions Divisions à activer
     * @return double Budget recommandé
     */
    /**
     * @brief Calcule le budget initial selon les divisions choisies
     * @param divisions Divisions à activer
     * @return double Budget recommandé
     */
    // double calculateInitialBudget(const std::vector<models::DivisionType>& divisions); // à réécrire avec ORM
};

} // namespace services