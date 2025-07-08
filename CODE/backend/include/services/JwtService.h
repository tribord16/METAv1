/**
 * @file JwtService.h
 * @brief Service de gestion des JWT tokens
 */

#pragma once
#include "models/User.h"
#include <string>
#include <optional>

namespace services {

struct JwtPayload {
    int user_id;
    std::string username;
    std::string email;
};

class JwtService {
public:
    // Générer un token JWT
    static std::string generateToken(const models::User& user);
    
    // Vérifier et décoder un token
    static std::optional<JwtPayload> verifyToken(const std::string& token);
    
    // Extraire le token du header Authorization
    static std::string extractTokenFromHeader(const std::string& authHeader);

private:
    static std::string getSecret();
};

} // namespace services