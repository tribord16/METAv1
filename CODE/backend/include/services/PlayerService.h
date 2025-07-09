/**
 * @file PlayerService.h
 * @brief Service joueurs - logique métier recrutement et gestion
 * @author MetaLeague Backend Team
 * @date 2025
 * @version 1.0
 *
 * RESPONSABILITÉS :
 * - Gérer pool global de talents
 * - Orchestrer système de recrutement
 * - Appliquer progression des joueurs
 * - Calculer synergies d'équipe
 * - Gérer contrats et négociations
 */

#pragma once

#include "repositories/PlayerRepository.h"
#include "dto/player/ScoutingRequest.h"
#include "dto/player/TeamComposition.h"
#include <functional>
#include <memory>

namespace services {

/**
 * @class PlayerService
 * @brief Service principal pour gestion des joueurs
 */
class PlayerService {
private:
    std::shared_ptr<repositories::PlayerRepository> playerRepo_;

public:
    /**
     * @brief Constructeur
     * @param playerRepo Repository des joueurs
     */
    explicit PlayerService(std::shared_ptr<repositories::PlayerRepository> playerRepo);

    /**
     * @brief Recherche de talents selon critères (scouting system)
     * @param request Critères de recherche détaillés
     * @param callback Fonction appelée avec les résultats
     * 
     * RECHERCHE AVANCÉE :
     * ```cpp
     * ScoutingRequest req;
     * req.position = PlayerPosition::MID;
     * req.min_overall = 75;
     * req.max_salary = 20000;
     * req.max_age = 22;
     * req.sort_by = "overall_rating";
     * 
     * playerService->scoutPlayers(req, [](const auto& results) {
     *     displayScoutingResults(results);
     * });
     * ```
     */
    void scoutPlayers(const dto::player::ScoutingRequest& request,
                     std::function<void(const std::vector<models::Player>&)> callback);

    /**
     * @brief Négocie et signe un joueur
     * @param playerId ID du joueur à recruter
     * @param corpId ID de la corporation
     * @param offerDetails Détails de l'offre (salaire, durée, bonus)
     * @param callback Fonction appelée avec succès/échec + détails
     * 
     * PROCESSUS NÉGOCIATION :
     * 1. Validation faisabilité (budget, roster space)
     * 2. Calcul attractivité offre vs demandes joueur
     * 3. Facteurs externes (réputation corp, compétition)
     * 4. Résolution négociation (accepté/refusé/contre-offre)
     * 5. Signature contrat si accepté
     */
    void negotiatePlayer(int playerId, int corpId, const Json::Value& offerDetails,
                        std::function<void(bool, const std::string&)> callback);

    /**
     * @brief Analyse la composition d'équipe actuelle
     * @param corpId ID de la corporation
     * @param callback Fonction appelée avec l'analyse complète
     * 
     * ANALYSE FOURNIE :
     * - Rating global équipe (0-100)
     * - Synergies par lane (Top-Jungle, Mid-Jungle, etc.)
     * - Points faibles identifiés
     * - Suggestions d'amélioration
     * - Comparaison vs meta actuelle
     * 
     * EXEMPLE RÉSULTAT :
     * ```json
     * {
     *   "overall_rating": 78,
     *   "synergies": {
     *     "top_jungle": 85,
     *     "mid_jungle": 72,
     *     "bot_lane": 91
     *   },
     *   "weaknesses": ["Lack of engage", "Poor late game"],
     *   "suggestions": ["Consider tank support", "Train macro skills"]
     * }
     * ```
     */
    void analyzeTeamComposition(int corpId,
                               std::function<void(const dto::player::TeamComposition&)> callback);

    /**
     * @brief Génère automatiquement des nouveaux talents
     * @param count Nombre de joueurs à générer
     * @param callback Fonction appelée quand terminé
     * 
     * GÉNÉRATION INTELLIGENTE :
     * - Distribution réaliste par position (20% chaque)
     * - Courbe normale de talent (majorité moyenne, peu d'élites)
     * - Âges cohérents esports (pic 16-19 ans pour rookies)
     * - Diversité géographique et culturelle
     * - Progression potential caché
     */
    void generateNewTalents(int count,
                           std::function<void(int)> callback);

    /**
     * @brief Simule progression mensuelle des joueurs
     * @param corpId ID de la corporation
     * @param callback Fonction appelée avec rapport de progression
     * 
     * FACTEURS PROGRESSION :
     * - Âge (peak 20-24, déclin après 26)
     * - Temps de jeu (réguliers progressent plus)
     * - Qualité coaching (impact direct)
     * - Facilities (gaming house, équipement)
     * - Team chemistry (environnement positif)
     * - Individual training (focus personnel)
     * 
     * RÉSULTAT :
     * - Liste joueurs avec gains/pertes de stats
     * - Nouveau potentiel révélé
     * - Changements de forme
     * - Évolution valeur marchande
     */
    void simulateMonthlyProgression(int corpId,
                                   std::function<void(const Json::Value&)> callback);

    /**
     * @brief Libère un joueur (fin de contrat ou release)
     * @param playerId ID du joueur
     * @param releaseType Type de libération ("contract_end", "mutual", "release")
     * @param callback Fonction appelée avec succès/échec
     * 
     * TYPES DE LIBÉRATION :
     * - "contract_end": Fin naturelle, pas de pénalité
     * - "mutual": Accord mutuel, compensation mineure
     * - "release": Corporation libère, pénalité buyout
     */
    void releasePlayer(int playerId, const std::string& releaseType,
                      std::function<void(bool, double)> callback);

private:
    /**
     * @brief Calcule l'attractivité d'une offre pour un joueur
     * @param player Le joueur concerné
     * @param offer Détails de l'offre
     * @param corpReputation Réputation de la corporation
     * @return double Score d'attractivité 0-100
     */
    double calculateOfferAttractiveness(const models::Player& player,
                                       const Json::Value& offer,
                                       int corpReputation);

    /**
     * @brief Calcule les synergies entre deux joueurs
     * @param player1 Premier joueur
     * @param player2 Deuxième joueur
     * @return double Score de synergie 0-100
     */
    double calculatePlayerSynergy(const models::Player& player1,
                                 const models::Player& player2);
};

} // namespace services