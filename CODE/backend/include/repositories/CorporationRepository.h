/**
 * @file CorporationRepository.h
 * @brief Repository pour gestion corporations - accès base de données
 * @author MetaLeague Backend Team
 * @date 2025
 * @version 1.0
 *
 * RESPONSABILITÉS :
 * - CRUD complet sur table corporations
 * - Requêtes spécialisées (par utilisateur, par budget, etc.)
 * - Intégrité référentielle avec users
 * - Optimisation performance (index, requêtes)
 *
 * UTILISATION TYPIQUE :
 * ```cpp
 * auto corpRepo = std::make_shared<CorporationRepository>(dbClient);
 * 
 * // Créer corporation
 * Corporation newCorp(userId, "Team Quantum");
 * corpRepo->create(newCorp, [](const Corporation& created) {
 *     Logger::info("Corporation created: " + created.name);
 * });
 * 
 * // Lister corporations du joueur
 * corpRepo->findByUser(userId, [](const std::vector<Corporation>& corps) {
 *     for (const auto& corp : corps) {
 *         std::cout << corp.name << " - " << corp.budget << "€" << std::endl;
 *     }
 * });
 * ```
 */

#pragma once
#include <models/Corporations.h>
#include <drogon/orm/DbClient.h>
#include <functional>
#include <optional>
#include <vector>

// Alias pour éviter d'écrire le namespace complet partout
using Corporations = drogon_model::meta_league_dev::Corporations;

namespace repositories {

/**
 * @class CorporationRepository
 * @brief Repository pour toutes les opérations sur les corporations
 *
 * Encapsule l'accès aux données des corporations avec :
 * - CRUD complet et optimisé
 * - Requêtes métier spécialisées
 * - Gestion des transactions financières
 * - Validation des contraintes business
 */
class CorporationRepository {
private:
    drogon::orm::DbClientPtr db_;

public:
    /**
     * @brief Constructeur
     * @param db Client de base de données Drogon
     */
    explicit CorporationRepository(drogon::orm::DbClientPtr db);

    /**
     * @brief Crée une nouvelle corporation
     * @param corp Corporation à créer
     * @param onSuccess Callback avec corporation créée (ID assigné)
     * @param onError Callback d'erreur avec message
     * 
     * VALIDATIONS :
     * - Nom unique par utilisateur
     * - Budget initial > 0
     * - Au moins une division active
     * 
     * EXEMPLE :
     * ```cpp
     * Corporation newCorp(userId, "Team Quantum", 1000000.0);
     * corpRepo->create(newCorp, 
     *     [](const Corporation& created) {
     *         // Corporation créée avec ID
     *         redirectToGame(created.id);
     *     },
     *     [](const std::string& error) {
     *         showError("Failed to create: " + error);
     *     }
     * );
     * ```
     */
    void create(const Corporations& corp,
                std::function<void(const Corporations&)> onSuccess,
                std::function<void(const std::string&)> onError);

    /**
     * @brief Trouve toutes les corporations d'un utilisateur
     * @param userId ID de l'utilisateur
     * @param callback Fonction appelée avec la liste
     * 
     * USAGE TYPIQUE :
     * ```cpp
     * corpRepo->findByUser(currentUserId, [](const auto& corporations) {
     *     if (corporations.empty()) {
     *         showCreateCorporationDialog();
     *     } else {
     *         showCorporationSelector(corporations);
     *     }
     * });
     * ```
     */
    void findByUser(int userId, 
                   std::function<void(const std::vector<Corporations>&)> callback);



    /**
    * @brief Trouve une corporation par son nom
    * @param name Nom de la corporation
    * @param callback Fonction appelée avec la corporation (ou nullopt)
    */
    void findByName(const std::string& name,
                    std::function<void(std::optional<Corporations>)> callback); 


    /**
     * @brief Trouve une corporation par ID
     * @param corpId ID de la corporation
     * @param callback Fonction appelée avec la corporation (ou nullopt)
     */
    void findById(int corpId,
                 std::function<void(std::optional<Corporations>)> callback);

    /**
     * @brief Met à jour une corporation
     * @param corp Corporation avec modifications
     * @param callback Fonction appelée avec succès/échec
     * 
     * CHAMPS MODIFIABLES :
     * - name (avec validation unicité)
     * - budget (transactions via TransactionRepository)
     * - reputation (selon événements jeu)
     * - divisions actives (unlocks progressifs)
     */
    void update(const Corporations& corp,
               std::function<void(bool)> callback);

    /**
     * @brief Supprime une corporation
     * @param corpId ID de la corporation
     * @param callback Fonction appelée avec succès/échec
     * 
     * CASCADE DELETE :
     * - Tous les joueurs (players.corporation_id)
     * - Toutes les transactions (transactions.corporation_id)
     * - État du jeu (game_states.corporation_id)
     */
    void remove(int corpId,
               std::function<void(bool)> callback);

    /**
     * @brief Met à jour le budget d'une corporation
     * @param corpId ID de la corporation
     * @param newBudget Nouveau budget
     * @param callback Fonction appelée avec succès/échec
     * 
     * VALIDATION :
     * - Budget >= 0 (pas de dette négative)
     * - Transaction atomique
     */
    void updateBudget(int corpId, double newBudget,
                     std::function<void(bool)> callback);

    /**
     * @brief Vérifie si un nom de corporation est disponible
     * @param userId ID de l'utilisateur
     * @param name Nom à vérifier
     * @param callback Fonction appelée avec disponibilité
     * 
     * RÈGLE BUSINESS :
     * - Nom unique par utilisateur (peut avoir même nom que autre user)
     * - Validation en temps réel côté frontend
     */
    void isNameAvailable(int userId, const std::string& name,
                        std::function<void(bool)> callback);




private:
    /**
     * @brief Convertit une ligne DB en objet Corporation
     * @param row Ligne de résultat SQL
     * @return Corporation Objet hydraté
     */
    // rowToCorporation supprimé, utiliser le constructeur Corporations(row) du modèle ORM
};

} // namespace repositories