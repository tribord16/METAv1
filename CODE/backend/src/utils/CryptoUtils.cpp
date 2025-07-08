/**
 * @file CryptoUtils.cpp
 * @brief Implémentation SIMPLE avec crypt() Linux
 */

#include "utils/CryptoUtils.h"
#include <crypt.h>  // BCrypt Linux système
#include <random>
#include <algorithm>
#include <cstdlib>

namespace utils {

std::string CryptoUtils::hashPassword(const std::string& password) {
    // Générer un salt aléatoire pour bcrypt
    const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789./";
    std::string salt = "$2b$12$";  // BCrypt format
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, sizeof(charset) - 2);
    
    // Générer 22 caractères de salt
    for (int i = 0; i < 22; ++i) {
        salt += charset[dis(gen)];
    }
    
    // Hasher avec crypt()
    char* hash = crypt(password.c_str(), salt.c_str());
    if (hash == nullptr) {
        return "";
    }
    
    return std::string(hash);
}

bool CryptoUtils::verifyPassword(const std::string& password, const std::string& hash) {
    if (hash.empty()) return false;
    
    char* result = crypt(password.c_str(), hash.c_str());
    if (result == nullptr) return false;
    
    return hash == std::string(result);
}

std::string CryptoUtils::generateRandomKey(size_t length) {
    const std::string chars = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, chars.size() - 1);
    
    std::string result;
    result.reserve(length);
    
    for (size_t i = 0; i < length; ++i) {
        result += chars[dis(gen)];
    }
    
    return result;
}

} // namespace utils