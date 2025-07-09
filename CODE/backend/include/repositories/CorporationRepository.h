// ============================================================================
// include/repositories/CorporationRepository.h - Corporation Data Access Layer
// ============================================================================

#pragma once
#include <memory>
#include <functional>
#include <optional>
#include <future>
#include <vector>
#include "models/Corporation.h"
#include "utils/DatabaseConnection.h"
#include "utils/Logger.h"

namespace repositories {

class CorporationRepository {
private:
    std::shared_ptr<utils::DatabaseConnection> dbPool_;

public:
    explicit CorporationRepository(std::shared_ptr<utils::DatabaseConnection> dbPool) 
        : dbPool_(std::move(dbPool)) {}

    // Create new corporation asynchronously
    std::future<std::optional<models::Corporation>> createCorporationAsync(const models::Corporation& corporation) {
        return std::async(std::launch::async, [this, corporation]() -> std::optional<models::Corporation> {
            try {
                utils::ConnectionGuard conn(*dbPool_);
                
                // Check if corporation name already exists for this user
                std::unique_ptr<sql::PreparedStatement> checkStmt(
                    conn->prepareStatement("SELECT COUNT(*) as count FROM corporations WHERE user_id = ? AND name = ?"));
                checkStmt->setInt(1, corporation.user_id);
                checkStmt->setString(2, corporation.name);
                
                std::unique_ptr<sql::ResultSet> checkRes(checkStmt->executeQuery());
                if (checkRes->next() && checkRes->getInt("count") > 0) {
                    utils::Logger::warn("Attempt to create corporation with existing name for user: " + std::to_string(corporation.user_id));
                    return std::nullopt;
                }
                
                // Insert new corporation
                std::unique_ptr<sql::PreparedStatement> stmt(
                    conn->prepareStatement(
                        "INSERT INTO corporations (user_id, name, budget, reputation, current_season, current_week, "
                        "esports_active, racing_active, tactical_active, innovation_active, created_at, last_played) "
                        "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, NOW(), NOW())"
                    ));
                
                stmt->setInt(1, corporation.user_id);
                stmt->setString(2, corporation.name);
                stmt->setDouble(3, corporation.budget);
                stmt->setInt(4, corporation.reputation);
                stmt->setInt(5, corporation.current_season);
                stmt->setInt(6, corporation.current_week);
                stmt->setBoolean(7, corporation.esports_active);
                stmt->setBoolean(8, corporation.racing_active);
                stmt->setBoolean(9, corporation.tactical_active);
                stmt->setBoolean(10, corporation.innovation_active);

                int affectedRows = stmt->executeUpdate();
                if (affectedRows == 0) {
                    utils::Logger::error("Failed to insert corporation: " + corporation.name);
                    return std::nullopt;
                }
                
                // Get the created corporation by finding the most recent one for this user
                return findCorporationByUserAndNameSync(corporation.user_id, corporation.name);
                
            } catch (sql::SQLException& e) {
                utils::Logger::error("Database error in createCorporation: " + std::string(e.what()));
                return std::nullopt;
            }
        });
    }

    // Find corporations by user ID
    std::future<std::vector<models::Corporation>> findCorporationsByUserAsync(int userId) {
        return std::async(std::launch::async, [this, userId]() -> std::vector<models::Corporation> {
            try {
                utils::ConnectionGuard conn(*dbPool_);
                
                std::unique_ptr<sql::PreparedStatement> stmt(
                    conn->prepareStatement(
                        "SELECT id, user_id, name, budget, reputation, current_season, current_week, "
                        "esports_active, racing_active, tactical_active, innovation_active, "
                        "UNIX_TIMESTAMP(created_at) as created_at, UNIX_TIMESTAMP(last_played) as last_played "
                        "FROM corporations WHERE user_id = ? ORDER BY created_at DESC"
                    ));
                
                stmt->setInt(1, userId);
                std::unique_ptr<sql::ResultSet> res(stmt->executeQuery());
                
                std::vector<models::Corporation> corporations;
                while (res->next()) {
                    corporations.push_back(resultSetToCorporation(*res));
                }
                
                return corporations;
                
            } catch (sql::SQLException& e) {
                utils::Logger::error("Database error in findCorporationsByUser: " + std::string(e.what()));
                return {};
            }
        });
    }

    // Find corporation by ID
    std::future<std::optional<models::Corporation>> findCorporationByIdAsync(int corporationId) {
        return std::async(std::launch::async, [this, corporationId]() -> std::optional<models::Corporation> {
            try {
                utils::ConnectionGuard conn(*dbPool_);
                
                std::unique_ptr<sql::PreparedStatement> stmt(
                    conn->prepareStatement(
                        "SELECT id, user_id, name, budget, reputation, current_season, current_week, "
                        "esports_active, racing_active, tactical_active, innovation_active, "
                        "UNIX_TIMESTAMP(created_at) as created_at, UNIX_TIMESTAMP(last_played) as last_played "
                        "FROM corporations WHERE id = ?"
                    ));
                
                stmt->setInt(1, corporationId);
                std::unique_ptr<sql::ResultSet> res(stmt->executeQuery());
                
                if (res->next()) {
                    return resultSetToCorporation(*res);
                }
                
                return std::nullopt;
                
            } catch (sql::SQLException& e) {
                utils::Logger::error("Database error in findCorporationById: " + std::string(e.what()));
                return std::nullopt;
            }
        });
    }

    // Update corporation
    std::future<bool> updateCorporationAsync(const models::Corporation& corporation) {
        return std::async(std::launch::async, [this, corporation]() -> bool {
            try {
                utils::ConnectionGuard conn(*dbPool_);
                
                std::unique_ptr<sql::PreparedStatement> stmt(
                    conn->prepareStatement(
                        "UPDATE corporations SET name = ?, budget = ?, reputation = ?, current_season = ?, "
                        "current_week = ?, esports_active = ?, racing_active = ?, tactical_active = ?, "
                        "innovation_active = ?, last_played = NOW() WHERE id = ?"
                    ));
                
                stmt->setString(1, corporation.name);
                stmt->setDouble(2, corporation.budget);
                stmt->setInt(3, corporation.reputation);
                stmt->setInt(4, corporation.current_season);
                stmt->setInt(5, corporation.current_week);
                stmt->setBoolean(6, corporation.esports_active);
                stmt->setBoolean(7, corporation.racing_active);
                stmt->setBoolean(8, corporation.tactical_active);
                stmt->setBoolean(9, corporation.innovation_active);
                stmt->setInt(10, corporation.id);
                
                int affectedRows = stmt->executeUpdate();
                return affectedRows > 0;
                
            } catch (sql::SQLException& e) {
                utils::Logger::error("Database error in updateCorporation: " + std::string(e.what()));
                return false;
            }
        });
    }

private:
    // Helper method to find corporation by user and name
    std::optional<models::Corporation> findCorporationByUserAndNameSync(int userId, const std::string& name) {
        try {
            utils::ConnectionGuard conn(*dbPool_);
            
            std::unique_ptr<sql::PreparedStatement> stmt(
                conn->prepareStatement(
                    "SELECT id, user_id, name, budget, reputation, current_season, current_week, "
                    "esports_active, racing_active, tactical_active, innovation_active, "
                    "UNIX_TIMESTAMP(created_at) as created_at, UNIX_TIMESTAMP(last_played) as last_played "
                    "FROM corporations WHERE user_id = ? AND name = ?"
                ));
            
            stmt->setInt(1, userId);
            stmt->setString(2, name);
            std::unique_ptr<sql::ResultSet> res(stmt->executeQuery());
            
            if (res->next()) {
                return resultSetToCorporation(*res);
            }
            
            return std::nullopt;
            
        } catch (sql::SQLException& e) {
            utils::Logger::error("Database error in findCorporationByUserAndName: " + std::string(e.what()));
            return std::nullopt;
        }
    }

    // Convert SQL ResultSet to Corporation model
    models::Corporation resultSetToCorporation(sql::ResultSet& res) {
        models::Corporation corp;
        corp.id = res.getInt("id");
        corp.user_id = res.getInt("user_id");
        corp.name = res.getString("name");
        corp.budget = res.getDouble("budget");
        corp.reputation = res.getInt("reputation");
        corp.current_season = res.getInt("current_season");
        corp.current_week = res.getInt("current_week");
        corp.esports_active = res.getBoolean("esports_active");
        corp.racing_active = res.getBoolean("racing_active");
        corp.tactical_active = res.getBoolean("tactical_active");
        corp.innovation_active = res.getBoolean("innovation_active");
        
        auto created_timestamp = res.getInt64("created_at");
        auto updated_timestamp = res.getInt64("last_played");
        
        corp.created_at = std::chrono::system_clock::from_time_t(created_timestamp);
        corp.last_played = std::chrono::system_clock::from_time_t(updated_timestamp);
        
        return corp;
    }
};

} // namespace repositories