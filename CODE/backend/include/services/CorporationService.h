// ============================================================================
// include/services/CorporationService.h - Corporation Business Logic
// ============================================================================

#pragma once
#include <memory>
#include <future>
#include <vector>
#include "repositories/CorporationRepository.h"
#include "dto/corporation/CreateCorporationRequest.h"
#include "dto/corporation/CorporationResponse.h"
#include "dto/common/ApiResponse.h" // Correction: Ajout de l'inclusion pour ApiResponse
#include "utils/Logger.h"

namespace services {

class CorporationService {
private:
    std::shared_ptr<repositories::CorporationRepository> corpRepo_;

public:
    explicit CorporationService(std::shared_ptr<repositories::CorporationRepository> corpRepo) 
        : corpRepo_(std::move(corpRepo)) {}

    // Create new corporation
    std::future<dto::common::ApiResponse<dto::corporation::CorporationResponse>> createCorporationAsync(
            const dto::corporation::CreateCorporationRequest& request, int userId) {
        
        return std::async(std::launch::async, [this, request, userId]() -> dto::common::ApiResponse<dto::corporation::CorporationResponse> {
            try {
                // Validate request
                auto validationErrors = request.validate();
                if (!validationErrors.empty()) {
                    utils::Logger::warn("Corporation creation validation failed for user: " + std::to_string(userId));
                    return dto::common::ApiResponse<dto::corporation::CorporationResponse>::createError(
                        "Validation failed", validationErrors);
                }

                // Create corporation model
                models::Corporation corp;
                corp.user_id = userId;
                corp.name = request.name;
                corp.budget = request.initial_budget;
                corp.reputation = 50; // Starting reputation
                corp.current_season = 1;
                corp.current_week = 1;
                corp.esports_active = request.esports_active;
                corp.racing_active = request.racing_active;
                corp.tactical_active = request.tactical_active;
                corp.innovation_active = request.innovation_active;

                // Create corporation in database
                auto createdCorpOpt = corpRepo_->createCorporationAsync(corp).get();
                if (!createdCorpOpt.has_value()) {
                    utils::Logger::error("Failed to create corporation in database: " + request.name);
                    return dto::common::ApiResponse<dto::corporation::CorporationResponse>::createError(
                        "Failed to create corporation - name may already exist");
                }

                auto response = dto::corporation::CorporationResponse::fromModel(createdCorpOpt.value());
                utils::Logger::info("Corporation created successfully: " + request.name + " for user: " + std::to_string(userId));
                
                return dto::common::ApiResponse<dto::corporation::CorporationResponse>::createSuccess(
                    "Corporation created successfully", response);

            } catch (const std::exception& e) {
                utils::Logger::error("Exception in createCorporation: " + std::string(e.what()));
                return dto::common::ApiResponse<dto::corporation::CorporationResponse>::createError(
                    "Internal server error");
            }
        });
    }

    // Get corporations by user
    std::future<dto::common::ApiResponse<std::vector<dto::corporation::CorporationResponse>>> getCorporationsByUserAsync(int userId) {
        return std::async(std::launch::async, [this, userId]() -> dto::common::ApiResponse<std::vector<dto::corporation::CorporationResponse>> {
            try {
                auto corporations = corpRepo_->findCorporationsByUserAsync(userId).get();
                
                std::vector<dto::corporation::CorporationResponse> responses;
                for (const auto& corp : corporations) {
                    responses.push_back(dto::corporation::CorporationResponse::fromModel(corp));
                }

                utils::Logger::debug("Retrieved " + std::to_string(responses.size()) + " corporations for user: " + std::to_string(userId));
                
                return dto::common::ApiResponse<std::vector<dto::corporation::CorporationResponse>>::createSuccess(
                    "Corporations retrieved successfully", responses);

            } catch (const std::exception& e) {
                utils::Logger::error("Exception in getCorporationsByUser: " + std::string(e.what()));
                return dto::common::ApiResponse<std::vector<dto::corporation::CorporationResponse>>::createError(
                    "Internal server error");
            }
        });
    }

    // Get corporation by ID (with user ownership check)
    std::future<dto::common::ApiResponse<dto::corporation::CorporationResponse>> getCorporationByIdAsync(int corporationId, int userId) {
        return std::async(std::launch::async, [this, corporationId, userId]() -> dto::common::ApiResponse<dto::corporation::CorporationResponse> {
            try {
                auto corpOpt = corpRepo_->findCorporationByIdAsync(corporationId).get();
                if (!corpOpt.has_value()) {
                    utils::Logger::warn("Corporation not found: " + std::to_string(corporationId));
                    return dto::common::ApiResponse<dto::corporation::CorporationResponse>::createError(
                        "Corporation not found");
                }

                auto corp = corpOpt.value();
                
                // Check ownership
                if (corp.user_id != userId) {
                    utils::Logger::warn("Unauthorized access attempt to corporation: " + std::to_string(corporationId) + " by user: " + std::to_string(userId));
                    return dto::common::ApiResponse<dto::corporation::CorporationResponse>::createError(
                        "Access denied - you don't own this corporation");
                }

                auto response = dto::corporation::CorporationResponse::fromModel(corp);
                
                return dto::common::ApiResponse<dto::corporation::CorporationResponse>::createSuccess(
                    "Corporation retrieved successfully", response);

            } catch (const std::exception& e) {
                utils::Logger::error("Exception in getCorporationById: " + std::string(e.what()));
                return dto::common::ApiResponse<dto::corporation::CorporationResponse>::createError(
                    "Internal server error");
            }
        });
    }
};

} // namespace services