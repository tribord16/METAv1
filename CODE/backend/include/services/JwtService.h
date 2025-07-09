/**
 * @file JwtService.h
 * @brief Service de gestion des JWT tokens
 */

#pragma once
#include <models/Users.h>
#include <string>
#include <optional>

// Alias pour éviter d'écrire le namespace complet partout
using Users = drogon_model::meta_league_dev::Users;

namespace services {

struct JwtPayload {
    int user_id;
    std::string username;
    std::string email;
};

class JwtService {
public:
    // Générer un token JWT
    static std::string generateToken(const Users& user);
    
    // Vérifier et décoder un token
    static std::optional<JwtPayload> verifyToken(const std::string& token);
    
    // Extraire le token du header Authorization
    static std::string extractTokenFromHeader(const std::string& authHeader);

private:
    static std::string getSecret();
};

} // namespace services