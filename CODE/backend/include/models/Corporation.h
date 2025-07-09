// ============================================================================
// include/models/Corporation.h - Corporation Entity Model
// ============================================================================

#pragma once
#include <string>
#include <chrono>
#include <nlohmann/json.hpp>

namespace models {

struct Corporation {
    int id = 0;
    int user_id = 0;
    std::string name;
    double budget = 1000000.0;
    int reputation = 50;
    int current_season = 1;
    int current_week = 1;
    bool esports_active = true;
    bool racing_active = false;
    bool tactical_active = false;
    bool innovation_active = false;
    std::chrono::system_clock::time_point created_at;
    std::chrono::system_clock::time_point last_played;

    nlohmann::json toJson() const {
        return nlohmann::json{
            {"id", id},
            {"user_id", user_id},
            {"name", name},
            {"budget", budget},
            {"reputation", reputation},
            {"current_season", current_season},
            {"current_week", current_week},
            {"divisions", {
                {"esports_active", esports_active},
                {"racing_active", racing_active},
                {"tactical_active", tactical_active},
                {"innovation_active", innovation_active}
            }},
            {"created_at", std::chrono::duration_cast<std::chrono::seconds>(created_at.time_since_epoch()).count()},
            {"last_played", std::chrono::duration_cast<std::chrono::seconds>(last_played.time_since_epoch()).count()}
        };
    }
};

// Ajout : fonction to_json pour nlohmann::json
inline void to_json(nlohmann::json& j, const models::Corporation& corp) {
    j = corp.toJson();
}

} // namespace models