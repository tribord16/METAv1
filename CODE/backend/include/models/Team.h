/*/ include/models/Player.h
#pragma once
#include <string>
#include <chrono>

namespace models {

enum class PlayerPosition {
    TOP = 0,
    JUNGLE = 1, 
    MID = 2,
    ADC = 3,
    SUPPORT = 4
};

class Player {
public:
    int id;
    std::string name;
    std::string nickname;
    PlayerPosition position;
    int team_id; // 0 si pas d'équipe
    int skill_level; // 1-100
    double salary;
    std::string contract_end_date;
    int age;
    std::string nationality;
    bool is_available;
    std::chrono::system_clock::time_point created_at;
    
    Player() = default;
    Player(const std::string& name, const std::string& nickname, PlayerPosition pos);
    
    bool isValid() const;
    std::string getPositionString() const;
    bool isUnderContract() const;
    double getMarketValue() const;
};

} // namespace models*/