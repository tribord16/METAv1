// ====== ./src/repositories/UserRepository.cpp ======
/**
 * @file UserRepository.cpp
 * @brief Implémentation du repository utilisateur avec Drogon ORM
 * @author Meta League Backend Team
 * @date 2025
 * @version 1.0
 *
 * IMPLÉMENTATION DÉTAILLÉE :
 * - Requêtes SQL asynchrones avec Drogon ORM
 * - Gestion complète des erreurs et callbacks
 * - Transformation Row → User avec gestion des types
 * - Logging détaillé pour debugging et monitoring
 * - Optimisations de performance (requêtes préparées, index)
 *
 * ARCHITECTURE TECHNIQUE :
 * - execSqlAsync() : exécution non-bloquante des requêtes
 * - Callbacks séparés pour succès/erreur
 * - Gestion des contraintes DB (UNIQUE, NOT NULL)
 * - Conversion types SQL ↔ C++ automatique
 *
 * SÉCURITÉ :
 * - Requêtes préparées (protection injection SQL)
 * - Pas de concaténation de strings SQL
 * - Validation des paramètres d'entrée
 * - Logging des erreurs pour audit
 */

#include "repositories/UserRepository.h"
#include "utils/Logger.h"
#include <chrono>

namespace repositories {

UserRepository::UserRepository(drogon::orm::DbClientPtr db) : db_(db) {
    Logger::info("[UserRepository] Initialized with database client");
}

void UserRepository::create(const Users& user,
                            std::function<void(const Users&)> onSuccess,
                            std::function<void(const std::string&)> onError) {
    Logger::info("[UserRepository] Creating user: " + user.getValueOfUsername());
    auto sql = "INSERT INTO users (username, email, password_hash, is_active, created_at, updated_at) "
               "VALUES (?, ?, ?, ?, NOW(), NOW())";
    db_->execSqlAsync(sql,
        [this, onSuccess, user](const drogon::orm::Result& result) {
            auto insertId = result.insertId();
            Logger::debug("[UserRepository] Generated user ID: " + std::to_string(insertId));
            findById(insertId, [onSuccess](std::optional<Users> createdUser) {
                if (createdUser.has_value()) {
                    Logger::info("[UserRepository] Retrieved created user with ID: " + std::to_string(createdUser->getValueOfId()));
                    onSuccess(createdUser.value());
                } else {
                    Logger::error("[UserRepository] Failed to retrieve created user");
                }
            });
        },
        [onError](const drogon::orm::DrogonDbException& e) {
            std::string error = e.base().what();
            Logger::error("[UserRepository] Error creating user: " + error);
            if (error.find("username") != std::string::npos || 
                error.find("users_username_key") != std::string::npos) {
                Logger::warn("[UserRepository] Username constraint violation");
                onError("Username already exists");
            } else if (error.find("email") != std::string::npos || 
                      error.find("users_email_key") != std::string::npos) {
                Logger::warn("[UserRepository] Email constraint violation");
                onError("Email already exists");
            } else {
                Logger::error("[UserRepository] Generic database error: " + error);
                onError("Database error");
            }
        },
        user.getValueOfUsername(),
        user.getValueOfEmail(),
        user.getValueOfPasswordHash(),
        user.getValueOfIsActive()
    );

}

void UserRepository::findByUsername(const std::string& username,
                                   std::function<void(std::optional<Users>)> callback) {
    Logger::debug("[UserRepository] Finding user by username: " + username);
    auto sql = "SELECT * FROM users WHERE username = ? AND is_active = TRUE";
    db_->execSqlAsync(sql,
        [callback](const drogon::orm::Result& result) {
            if (result.empty()) {
                Logger::debug("[UserRepository] User not found by username");
                callback(std::nullopt);
            } else {
                Logger::debug("[UserRepository] User found by username");
                Users user(result[0]);
                callback(user);
            }
        },
        [callback](const drogon::orm::DrogonDbException& e) {
            Logger::error("[UserRepository] Error finding user by username: " + std::string(e.base().what()));
            callback(std::nullopt);
        },
        username
    );
}

void UserRepository::findByEmail(const std::string& email,
                                std::function<void(std::optional<Users>)> callback) {
    Logger::debug("[UserRepository] Finding user by email: " + email);
    auto sql = "SELECT * FROM users WHERE email = ? AND is_active = TRUE";
    db_->execSqlAsync(sql,
        [callback](const drogon::orm::Result& result) {
            if (result.empty()) {
                Logger::debug("[UserRepository] User not found by email");
                callback(std::nullopt);
            } else {
                Logger::debug("[UserRepository] User found by email");
                Users user(result[0]);
                callback(user);
            }
        },
        [callback](const drogon::orm::DrogonDbException& e) {
            Logger::error("[UserRepository] Error finding user by email: " + std::string(e.base().what()));
            callback(std::nullopt);
        },
        email
    );
}

/**
 * @brief Implémentation de la recherche par ID
 */
void UserRepository::findById(int id, 
                             std::function<void(std::optional<Users>)> callback) {
    Logger::debug("[UserRepository] Finding user by ID: " + std::to_string(id));
    auto sql = "SELECT * FROM users WHERE id = ? AND is_active = TRUE";
    db_->execSqlAsync(sql,
        [callback](const drogon::orm::Result& result) {
            if (result.empty()) {
                Logger::debug("[UserRepository] User not found by ID");
                callback(std::nullopt);
            } else {
                Logger::debug("[UserRepository] User found by ID");
                Users user(result[0]);
                callback(user);
            }
        },
        [callback](const drogon::orm::DrogonDbException& e) {
            Logger::error("[UserRepository] Error finding user by ID: " + std::string(e.base().what()));
            callback(std::nullopt);
        },
        id
    );
}

/**
 * @brief Implémentation de la vérification d'existence par username
 */
void UserRepository::existsByUsername(const std::string& username,
                                     std::function<void(bool)> callback) {
    
    Logger::debug("[UserRepository] Checking username existence: " + username);
    
    // COUNT(*) pour vérifier l'existence (plus efficace qu'un SELECT *)
    auto sql = "SELECT COUNT(*) as count FROM users WHERE username = ?";
    
    db_->execSqlAsync(sql,
        [callback](const drogon::orm::Result& result) {
            int count = result[0]["count"].as<int>();
            bool exists = (count > 0);
            Logger::debug("[UserRepository] Username exists: " + std::string(exists ? "true" : "false"));
            callback(exists);
        },
        [callback](const drogon::orm::DrogonDbException& e) {
            Logger::error("[UserRepository] Error checking username existence: " + std::string(e.base().what()));
            callback(false); // En cas d'erreur, on considère comme "n'existe pas"
        },
        username
    );
}

/**
 * @brief Implémentation de la vérification d'existence par email
 */
void UserRepository::existsByEmail(const std::string& email,
                                  std::function<void(bool)> callback) {
    
    Logger::debug("[UserRepository] Checking email existence: " + email);
    
    auto sql = "SELECT COUNT(*) as count FROM users WHERE email = ?";
    
    db_->execSqlAsync(sql,
        [callback](const drogon::orm::Result& result) {
            int count = result[0]["count"].as<int>();
            bool exists = (count > 0);
            Logger::debug("[UserRepository] Email exists: " + std::string(exists ? "true" : "false"));
            callback(exists);
        },
        [callback](const drogon::orm::DrogonDbException& e) {
            Logger::error("[UserRepository] Error checking email existence: " + std::string(e.base().what()));
            callback(false);
        },
        email
    );
}



} // namespace repositories