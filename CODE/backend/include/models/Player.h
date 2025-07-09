// ============================================================================
// include/models/Player.h - Player Entity Model
// ============================================================================

#pragma once
#include <string>
#include <optional>
#include <chrono>
#include <nlohmann/json.hpp>

namespace models {

enum class Position {
    TOP, JUNGLE, MID, ADC, SUPPORT,  // Cyber division
    PILOT,                           // Racing division
    TACTICIAN,                       // Tactical division
    INNOVATOR                        // Innovation division
};

enum class DivisionType {
    CYBER, RACING, TACTICAL, INNOVATION
};

struct PlayerStats {
    int overall_rating = 70;
    int mechanical_skill = 70;
    int game_sense = 70;
    int teamwork = 70;
    int consistency = 70;

    nlohmann::json toJson() const {
        return nlohmann::json{
            {"overall_rating", overall_rating},
            {"mechanical_skill", mechanical_skill},
            {"game_sense", game_sense},
            {"teamwork", teamwork},
            {"consistency", consistency}
        };
    }
};

struct Player {
    int id = 0;
    std::optional<int> corporation_id;
    std::string first_name;
    std::string last_name;
    std::string nickname;
    Position position;
    DivisionType division_type;
    PlayerStats stats;
    int age;
    double salary = 50000.0;
    std::optional<std::chrono::system_clock::time_point> contract_end_date;
    bool is_available = true;
    std::chrono::system_clock::time_point created_at;
    std::chrono::system_clock::time_point updated_at;

    std::string getFullName() const {
        return first_name + " '" + nickname + "' " + last_name;
    }

    nlohmann::json toJson() const {
        nlohmann::json json = {
            {"id", id},
            {"first_name", first_name},
            {"last_name", last_name},
            {"nickname", nickname},
            {"full_name", getFullName()},
            {"position", static_cast<int>(position)},
            {"division_type", static_cast<int>(division_type)},
            {"stats", stats.toJson()},
            {"age", age},
            {"salary", salary},
            {"is_available", is_available},
            {"created_at", std::chrono::duration_cast<std::chrono::seconds>(created_at.time_since_epoch()).count()},
            {"updated_at", std::chrono::duration_cast<std::chrono::seconds>(updated_at.time_since_epoch()).count()}
        };

        if (corporation_id.has_value()) {
            json["corporation_id"] = corporation_id.value();
        }

        if (contract_end_date.has_value()) {
            json["contract_end_date"] = std::chrono::duration_cast<std::chrono::seconds>(
                contract_end_date.value().time_since_epoch()).count();
        }

        return json;
    }
};

// Helper functions for enum conversions
inline std::string positionToString(Position pos) {
    switch (pos) {
        case Position::TOP: return "TOP";
        case Position::JUNGLE: return "JUNGLE";
        case Position::MID: return "MID";
        case Position::ADC: return "ADC";
        case Position::SUPPORT: return "SUPPORT";
        case Position::PILOT: return "PILOT";
        case Position::TACTICIAN: return "TACTICIAN";
        case Position::INNOVATOR: return "INNOVATOR";
        default: return "UNKNOWN";
    }
}

inline Position stringToPosition(const std::string& str) {
    if (str == "TOP") return Position::TOP;
    if (str == "JUNGLE") return Position::JUNGLE;
    if (str == "MID") return Position::MID;
    if (str == "ADC") return Position::ADC;
    if (str == "SUPPORT") return Position::SUPPORT;
    if (str == "PILOT") return Position::PILOT;
    if (str == "TACTICIAN") return Position::TACTICIAN;
    if (str == "INNOVATOR") return Position::INNOVATOR;
    return Position::MID; // default
}

inline std::string divisionTypeToString(DivisionType type) {
    switch (type) {
        case DivisionType::CYBER: return "CYBER";
        case DivisionType::RACING: return "RACING";
        case DivisionType::TACTICAL: return "TACTICAL";
        case DivisionType::INNOVATION: return "INNOVATION";
        default: return "UNKNOWN";
    }
}

inline DivisionType stringToDivisionType(const std::string& str) {
    if (str == "CYBER") return DivisionType::CYBER;
    if (str == "RACING") return DivisionType::RACING;
    if (str == "TACTICAL") return DivisionType::TACTICAL;
    if (str == "INNOVATION") return DivisionType::INNOVATION;
    return DivisionType::CYBER; // default
}

} // namespace models