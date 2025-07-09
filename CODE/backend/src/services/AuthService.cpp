/**
 * @file AuthService.cpp
 * @brief Implémentation AuthService
 */

#include "services/AuthService.h"
#include "services/JwtService.h"
#include "utils/CryptoUtils.h"
#include "utils/Logger.h"
#include "dto/user/UserResponse.h"

namespace services {

AuthService::AuthService(std::shared_ptr<repositories::UserRepository> userRepo)
    : userRepo_(userRepo) {}

void AuthService::registerUser(const dto::auth::RegisterRequest& request,
                              std::function<void(const dto::auth::AuthResponse&)> callback) {
    
    Logger::info("Starting user registration for: " + request.username);
    
    // 1. Vérifier si l'utilisateur peut s'inscrire
    canRegister(request, [this, request, callback](bool canReg, std::vector<std::string> errors) {
        if (!canReg) {
            Logger::warn("Registration failed for: " + request.username + " - validation errors");
            callback(dto::auth::AuthResponse::error("Registration failed"));
            return;
        }
        
        // 2. Hasher le mot de passe
        std::string hashedPassword = utils::CryptoUtils::hashPassword(request.password);
        
        // 3. Créer l'utilisateur
        Users newUser;
        newUser.setUsername(request.username);
        newUser.setEmail(request.email);
        newUser.setPasswordHash(hashedPassword);
        newUser.setIsActive(true);
        
        userRepo_->create(newUser,
            [this, callback](const Users& createdUser) {
                Logger::info("User registered successfully: " + createdUser.getValueOfUsername());
                
                // 4. Générer le token JWT
                std::string token = JwtService::generateToken(createdUser);
                
                // 5. Créer la réponse
                auto userResponse = dto::user::UserResponse::fromUser(createdUser);
                auto authResponse = dto::auth::AuthResponse::success(token, userResponse);
                
                callback(authResponse);
            },
            [callback](const std::string& error) {
                Logger::error("User registration failed: " + error);
                callback(dto::auth::AuthResponse::error("Registration failed: " + error));
            }
        );
    });
}

void AuthService::login(const dto::auth::LoginRequest& request,
                       std::function<void(const dto::auth::AuthResponse&)> callback) {
    
    Logger::info("Login attempt for: " + request.username);
    
    // 1. Chercher l'utilisateur
    userRepo_->findByUsername(request.username, [this, request, callback](std::optional<Users> userOpt) {
        if (!userOpt.has_value()) {
            Logger::warn("Login failed - user not found: " + request.username);
            callback(dto::auth::AuthResponse::error("Invalid credentials"));
            return;
        }
        
        auto user = userOpt.value();
        
        // 2. Vérifier le mot de passe
        if (!utils::CryptoUtils::verifyPassword(request.password, user.getValueOfPasswordHash())) {
            Logger::warn("Login failed - wrong password for: " + request.username);
            callback(dto::auth::AuthResponse::error("Invalid credentials"));
            return;
        }
        
        Logger::info("Login successful for: " + user.getValueOfUsername());
        
        // 3. Générer le token JWT
        std::string token = JwtService::generateToken(user);
        
        // 4. Créer la réponse
        auto userResponse = dto::user::UserResponse::fromUser(user);
        auto authResponse = dto::auth::AuthResponse::success(token, userResponse);
        
        callback(authResponse);
    });
}

void AuthService::canRegister(const dto::auth::RegisterRequest& request,
                             std::function<void(bool, std::vector<std::string>)> callback) {
    
    // 1. Validation du DTO
    if (!request.isValid()) {
        callback(false, request.getErrors());
        return;
    }
    
    // 2. Vérifier si username existe déjà
    userRepo_->existsByUsername(request.username, [this, request, callback](bool usernameExists) {
        if (usernameExists) {
            callback(false, {"Username already exists"});
            return;
        }
        
        // 3. Vérifier si email existe déjà
        userRepo_->existsByEmail(request.email, [callback](bool emailExists) {
            if (emailExists) {
                callback(false, {"Email already exists"});
                return;
            }
            
            // 4. Tout est OK
            callback(true, {});
        });
    });
}

} // namespace services