/**
 * @file User.h
 * @brief Modèle utilisateur - entité de base de données
 */

#pragma once
#include <string>
#include <chrono>

namespace models {

class User {
public:
    int id;
    std::string username;
    std::string email;
    std::string password_hash;
    bool is_active;
    std::chrono::system_clock::time_point created_at;
    std::chrono::system_clock::time_point updated_at;

    // Constructeurs
    User() = default;
    User(const std::string& username, const std::string& email, const std::string& password_hash);

    // Validation
    bool isValid() const;
    
    // Helpers
    std::string getCreatedAtString() const;
    std::string getUpdatedAtString() const;
};

} // namespace models