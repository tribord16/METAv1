/**
 * @file CorporationService.cpp
 * @brief Implémentation du service de gestion des corporations
 * @author Meta League Backend Team
 * @date 2025
 * @version 1.0
 * 
 * RESPONSABILITÉS :
 * - Orchestration de la création, gestion, et reporting des corporations
 * - Application des règles business (budgets, réputation, contraintes ligue)
 * - Intégration avec les autres services (Players, Matches, Events)
 * - Fourniture de DTOs propres pour l'API (jamais d'exposition directe
 * des modèles)
 * 
 * ARCHITECTURE :
 * Controller <-> CorporationService <-> Repositories <-> Database
 * 
 * BONNES PRATIQUES :
 * - Entrée : toujours via DTOs (CreateCorporationRequest, ...)
 * - Sortie : toujours via DTOs (CorporationResponse, CorporationDashboard, ...)
 * - Validation complète des données avant toute opération
 * - Gestion des erreurs métier avec messages clairs
 * - Logging détaillé pour audit et debugging
 * 
 * EXEMPLE D'UTILISATION :
 * ```cpp
 * auto corpRepo = std::make_shared<repositories::CorporationRepository>(dbClient);
// * auto playerRepo = std::make_shared<repositories::PlayerRepository>(dbClient); // désactivé migration ORM
 * auto corpService = std::make_shared<services::CorporationService>(corpRepo,
 * playerRepo);
 * 
 * // // Création d'une corporation
 * CreateCorporationRequest req;
 * req.name = "Team Quantum";
 * req.initial_budget = 1000000.0;
 * req.esports_active = true;
 * req.racing_active = false;
 * req.tactical_active = true;
 * req.innovation_active = false;
 * corpService->createCorporation(req, userId, [](const dto::corporation::Corporation
 * Response& response) {
 *    if (response.is_success) {
 *       // Corporation créée avec succès
 *      Logger::info("Corporation created: " + response.name);
 *      redirectToGameDashboard(response.id);
 *   } else {
 *      // Gérer l'erreur
 *     Logger::error("Error creating corporation: " + response.message);
 *  }
 * });
 * ``` OUBLIE PAS PLAYER REPO
 */


#include "services/CorporationService.h"
#include "utils/Logger.h"

using namespace services;
using namespace dto::corporation;


CorporationService::CorporationService(
    std::shared_ptr<repositories::CorporationRepository> corpRepo)
    : corpRepo_(std::move(corpRepo)) {
    if (!corpRepo_) {
        throw std::invalid_argument("Repositories cannot be null");
    }
}

// Correction de la création (3 arguments, plus d'optional)
void CorporationService::createCorporation(
    const dto::corporation::CreateCorporationRequest& request,
    int userId,
    std::function<void(const dto::corporation::CorporationResponse&)> callback)
{
    // 1. Validation métier
    if (!request.isValid()) {
        auto errors = request.getErrors();
        std::string errorMsg = "Validation failed:";
        for (const auto& err : errors) errorMsg += "\n- " + err;
        Logger::warn(errorMsg);
        callback(dto::corporation::CorporationResponse::error(errorMsg));
        return;
    }

    // 2. Calcul du budget initial
    double initialBudget = request.initial_budget;
    if (request.esports_active) initialBudget += 500000.0;
    if (request.racing_active) initialBudget += 300000.0;
    if (request.tactical_active) initialBudget += 200000.0;
    if (request.innovation_active) initialBudget += 100000.0;
    if (initialBudget <= 0) {
        callback(dto::corporation::CorporationResponse::error("Budget initial insuffisant"));
        return;
    }

    // 3. Vérification unicité du nom
    corpRepo_->findByName(request.name, [this, request, userId, initialBudget, callback](std::optional<Corporations> existingCorp) {
        if (existingCorp.has_value()) {
            callback(dto::corporation::CorporationResponse::error("Nom de corporation déjà utilisé"));
            return;
        }
        
        // 4. Création du modèle
        Corporations newCorp;
        newCorp.setUserId(userId);
        newCorp.setName(request.name);
        newCorp.setBudget(std::to_string(initialBudget));
        newCorp.setEsportsActive(request.esports_active);
        newCorp.setRacingActive(request.racing_active);
        newCorp.setTacticalActive(request.tactical_active);
        newCorp.setInnovationActive(request.innovation_active);
        
        // 5. Persistance
        corpRepo_->create(newCorp,
            [callback](const Corporations& created) {
                callback(dto::corporation::CorporationResponse::fromModel(created));
            },
            [callback](const std::string& error) {
                callback(dto::corporation::CorporationResponse::error(error));
            }
        );
    });
}

void CorporationService::listCorporationsByUser(int userId,
    std::function<void(const std::vector<dto::corporation::CorporationResponse>&)> callback) {
    corpRepo_->findByUser(userId, [callback](const std::vector<Corporations>& corps) {
        std::vector<dto::corporation::CorporationResponse> dtos;
        for (const auto& corp : corps) {
            dtos.push_back(dto::corporation::CorporationResponse::fromModel(corp));
        }
        callback(dtos);
    });
}

void CorporationService::getCorporationById(int corpId, int userId,
    std::function<void(const dto::corporation::CorporationResponse&)> callback) {
    corpRepo_->findById(corpId, [this, userId, callback](std::optional<Corporations> corpOpt) {
        if (!corpOpt || corpOpt->getValueOfUserId() != userId) {
            callback(dto::corporation::CorporationResponse::error("Not found or forbidden"));
            return;
        }
        callback(dto::corporation::CorporationResponse::fromModel(*corpOpt));
    });
}

// Correction de la mise à jour (remplace updated_at par last_played)
void CorporationService::updateCorporation(int corpId, const dto::corporation::CreateCorporationRequest& request, int userId,
    std::function<void(const dto::corporation::CorporationResponse&)> callback) {
    corpRepo_->findById(corpId, [this, request, userId, callback](std::optional<Corporations> corpOpt) {
        if (!corpOpt || corpOpt->getValueOfUserId() != userId) {
            callback(dto::corporation::CorporationResponse::error("Not found or forbidden"));
            return;
        }
        Corporations corp = *corpOpt;
        corp.setName(request.name);
        corp.setBudget(std::to_string(request.initial_budget));
        corp.setEsportsActive(request.esports_active);
        corp.setRacingActive(request.racing_active);
        corp.setTacticalActive(request.tactical_active);
        corp.setInnovationActive(request.innovation_active);
        corp.setLastPlayed(trantor::Date::now());
        corpRepo_->update(corp, [callback, corp](bool success) {
            if (success) {
                callback(dto::corporation::CorporationResponse::fromModel(corp));
            } else {
                callback(dto::corporation::CorporationResponse::error("Update failed"));
            }
        });
    });
}

void CorporationService::deleteCorporation(int corpId, int userId,
    std::function<void(bool)> callback) {
    corpRepo_->findById(corpId, [this, userId, callback, corpId](std::optional<Corporations> corpOpt) {
        if (!corpOpt || corpOpt->getValueOfUserId() != userId) {
            callback(false);
            return;
        }
        corpRepo_->remove(corpId, [callback](bool success) {
            callback(success);
        });
    });
}

void CorporationService::getCorporationDashboard(int corpId, int userId,
    std::function<void(const dto::corporation::CorporationDashboard&)> callback) {
    corpRepo_->findById(corpId, [this, userId, callback](std::optional<Corporations> corpOpt) {
        if (!corpOpt || corpOpt->getValueOfUserId() != userId) {
            callback(dto::corporation::CorporationDashboard{}); // ou un dashboard vide/erreur
            return;
        }
        // TODO: Remplir le dashboard avec toutes les infos nécessaires
        dto::corporation::CorporationDashboard dashboard;
        dashboard.corporation = *corpOpt;
        // ... compléter avec finances, team, events, etc.
        callback(dashboard);
    });
}

