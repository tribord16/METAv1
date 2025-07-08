/**
 * @file User.cpp
 * @brief Implémentation du modèle User
 */

#include "models/User.h"
#include <iomanip>
#include <sstream>

namespace models {

User::User(const std::string& username, const std::string& email, const std::string& password_hash)
    : id(0), username(username), email(email), password_hash(password_hash),
      is_active(true), created_at(std::chrono::system_clock::now()),
      updated_at(std::chrono::system_clock::now()) {}

bool User::isValid() const {
    return !username.empty() && !email.empty() && !password_hash.empty();
}

std::string User::getCreatedAtString() const {
    auto time_t = std::chrono::system_clock::to_time_t(created_at);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

std::string User::getUpdatedAtString() const {
    auto time_t = std::chrono::system_clock::to_time_t(updated_at);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

} // namespace models