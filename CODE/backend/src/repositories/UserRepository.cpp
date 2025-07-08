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

/**
 * @brief Constructeur : initialise le repository avec le client DB
 */
UserRepository::UserRepository(drogon::orm::DbClientPtr db) : db_(db) {
    Logger::info("[UserRepository] Initialized with database client");
}

/**
 * @brief Implémentation complète de la création d'utilisateur
 * 
 * SÉQUENCE DÉTAILLÉE :
 * 1. Préparation requête SQL avec placeholders (?)
 * 2. Exécution asynchrone avec paramètres bindés
 * 3. Gestion callback succès : récupération utilisateur créé
 * 4. Gestion callback erreur : analyse du type d'erreur
 * 5. Logging de toutes les opérations
 */
void UserRepository::create(const models::User& user,
                           std::function<void(const models::User&)> onSuccess,
                           std::function<void(const std::string&)> onError) {
    
    Logger::info("[UserRepository] Creating user: " + user.username);
    
    // Requête SQL préparée avec placeholders pour sécurité
    auto sql = "INSERT INTO users (username, email, password_hash, is_active, created_at, updated_at) "
               "VALUES (?, ?, ?, ?, NOW(), NOW())";
    
    /**
     * EXÉCUTION ASYNCHRONE avec callbacks spécialisés
     * 
     * execSqlAsync() prend :
     * - La requête SQL avec placeholders
     * - Callback de succès (Result)
     * - Callback d'erreur (DrogonDbException)  
     * - Les paramètres à binder (username, email, etc.)
     */
    db_->execSqlAsync(sql,
        // ==================== CALLBACK SUCCÈS ====================
        [this, onSuccess, user](const drogon::orm::Result& result) {
            Logger::info("[UserRepository] User created successfully: " + user.username);
            
            /**
             * RÉCUPÉRATION DE L'UTILISATEUR CRÉÉ
             * 
             * result.insertId() donne l'ID auto-généré par la DB
             * On fait une requête SELECT pour récupérer l'entité complète
             * avec tous les champs (timestamps, etc.)
             */
            auto insertId = result.insertId();
            Logger::debug("[UserRepository] Generated user ID: " + std::to_string(insertId));
            
            findById(insertId, [onSuccess](std::optional<models::User> createdUser) {
                if (createdUser.has_value()) {
                    Logger::info("[UserRepository] Retrieved created user with ID: " + std::to_string(createdUser->id));
                    onSuccess(createdUser.value());
                } else {
                    Logger::error("[UserRepository] Failed to retrieve created user");
                    // Note: ce cas ne devrait jamais arriver en fonctionnement normal
                }
            });
        },
        
        // ==================== CALLBACK ERREUR ====================
        [onError](const drogon::orm::DrogonDbException& e) {
            std::string error = e.base().what();
            Logger::error("[UserRepository] Error creating user: " + error);
            
            /**
             * ANALYSE DU TYPE D'ERREUR
             * 
             * PostgreSQL retourne des messages spécifiques pour les contraintes :
             * - "duplicate key value violates unique constraint \"users_username_key\""
             * - "duplicate key value violates unique constraint \"users_email_key\""
             * 
             * On analyse le message pour donner une erreur métier claire
             */
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
        
        // ==================== PARAMÈTRES BINDÉS ====================
        /**
         * Les paramètres sont bindés dans l'ordre des placeholders (?)
         * Drogon gère automatiquement l'échappement et la conversion de types
         */
        user.username, 
        user.email, 
        user.password_hash, 
        user.is_active
    );
}

/**
 * @brief Implémentation de la recherche par username
 */
void UserRepository::findByUsername(const std::string& username,
                                   std::function<void(std::optional<models::User>)> callback) {
    
    Logger::debug("[UserRepository] Finding user by username: " + username);
    
    // Requête avec filtre is_active pour sécurité
    auto sql = "SELECT * FROM users WHERE username = ? AND is_active = TRUE";
    
    db_->execSqlAsync(sql,
        // CALLBACK SUCCÈS
        [this, callback](const drogon::orm::Result& result) {
            if (result.empty()) {
                Logger::debug("[UserRepository] User not found by username");
                callback(std::nullopt);
            } else {
                Logger::debug("[UserRepository] User found by username");
                callback(rowToUser(result[0])); // Premier (et unique) résultat
            }
        },
        
        // CALLBACK ERREUR
        [callback](const drogon::orm::DrogonDbException& e) {
            Logger::error("[UserRepository] Error finding user by username: " + std::string(e.base().what()));
            callback(std::nullopt); // En cas d'erreur DB, on considère comme "non trouvé"
        },
        
        username
    );
}

/**
 * @brief Implémentation de la recherche par email
 */
void UserRepository::findByEmail(const std::string& email,
                                std::function<void(std::optional<models::User>)> callback) {
    
    Logger::debug("[UserRepository] Finding user by email: " + email);
    
    auto sql = "SELECT * FROM users WHERE email = ? AND is_active = TRUE";
    
    db_->execSqlAsync(sql,
        [this, callback](const drogon::orm::Result& result) {
            if (result.empty()) {
                Logger::debug("[UserRepository] User not found by email");
                callback(std::nullopt);
            } else {
                Logger::debug("[UserRepository] User found by email");
                callback(rowToUser(result[0]));
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
                             std::function<void(std::optional<models::User>)> callback) {
    
    Logger::debug("[UserRepository] Finding user by ID: " + std::to_string(id));
    
    auto sql = "SELECT * FROM users WHERE id = ? AND is_active = TRUE";
    
    db_->execSqlAsync(sql,
        [this, callback](const drogon::orm::Result& result) {
            if (result.empty()) {
                Logger::debug("[UserRepository] User not found by ID");
                callback(std::nullopt);
            } else {
                Logger::debug("[UserRepository] User found by ID");
                callback(rowToUser(result[0]));
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

/**
 * @brief Transformation Row SQL → User entity
 * 
 * CONVERSION DÉTAILLÉE DES TYPES :
 * - PostgreSQL INTEGER → C++ int
 * - PostgreSQL VARCHAR → C++ std::string  
 * - PostgreSQL BOOLEAN → C++ bool
 * - PostgreSQL TIMESTAMP → C++ std::chrono::time_point
 */
models::User UserRepository::rowToUser(const drogon::orm::Row& row) {
    Logger::debug("[UserRepository] Converting DB row to User entity");
    
    models::User user;
    
    // Extraction des champs basiques avec conversion automatique
    user.id = row["id"].as<int>();
    user.username = row["username"].as<std::string>();
    user.email = row["email"].as<std::string>();
    user.password_hash = row["password_hash"].as<std::string>();
    user.is_active = row["is_active"].as<bool>();
    
    /**
     * GESTION DES TIMESTAMPS
     * 
     * Pour l'instant, conversion simplifiée vers timestamp actuel
     * TODO: implémenter une vraie conversion string → time_point
     * 
     * Format PostgreSQL : "2025-07-08 14:30:15.123456"
     * Target C++ : std::chrono::system_clock::time_point
     */
    auto created_str = row["created_at"].as<std::string>();
    auto updated_str = row["updated_at"].as<std::string>();
    
    // Conversion temporaire (à améliorer)
    user.created_at = std::chrono::system_clock::now();
    user.updated_at = std::chrono::system_clock::now();
    
    Logger::debug("[UserRepository] Converted user ID " + std::to_string(user.id) + " successfully");
    
    return user;
}

} // namespace repositories