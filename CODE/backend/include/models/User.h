// ============================================================================
// include/models/User.h - User Entity Model
// ============================================================================

#pragma once
#include <string>
#include <chrono>
#include <nlohmann/json.hpp>

namespace models {

struct User {
    int id = 0;
    std::string username;
    std::string email;
    std::string password_hash;
    bool is_active = true;
    std::chrono::system_clock::time_point created_at;
    std::chrono::system_clock::time_point updated_at;

    // Convert to JSON (without password hash for security)
    nlohmann::json toJson() const {
        return nlohmann::json{
            {"id", id},
            {"username", username},
            {"email", email},
            {"is_active", is_active},
            {"created_at", std::chrono::duration_cast<std::chrono::seconds>(created_at.time_since_epoch()).count()},
            {"updated_at", std::chrono::duration_cast<std::chrono::seconds>(updated_at.time_since_epoch()).count()}
        };
    }
};

} // namespace models