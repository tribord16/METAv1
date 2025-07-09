// ============================================================================
// include/utils/PasswordUtils.h - BCrypt Password Hashing
// ============================================================================

#pragma once
#include <string>
#include <random>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/sha.h>
#include <iomanip>
#include <sstream>

namespace utils {

class PasswordUtils {
public:
    // Generate a secure random salt
    static std::string generateSalt(size_t length = 16) {
        std::vector<unsigned char> salt(length);
        if (RAND_bytes(salt.data(), length) != 1) {
            throw std::runtime_error("Failed to generate random salt");
        }
        
        std::stringstream ss;
        for (auto byte : salt) {
            ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte);
        }
        return ss.str();
    }

    // Hash password using PBKDF2 (more reliable than bcrypt for our use case)
    static std::string hashPassword(const std::string& password) {
        std::string salt = generateSalt();
        std::string hash = hashPasswordWithSalt(password, salt);
        return salt + ":" + hash;
    }

    // Verify password against stored hash
    static bool verifyPassword(const std::string& password, const std::string& storedHash) {
        size_t colonPos = storedHash.find(':');
        if (colonPos == std::string::npos) {
            return false;
        }
        
        std::string salt = storedHash.substr(0, colonPos);
        std::string hash = storedHash.substr(colonPos + 1);
        
        std::string computedHash = hashPasswordWithSalt(password, salt);
        return hash == computedHash;
    }

private:
    static std::string hashPasswordWithSalt(const std::string& password, const std::string& salt) {
        const int iterations = 100000;
        const int keyLength = 32;
        
        std::vector<unsigned char> derivedKey(keyLength);
        
        if (PKCS5_PBKDF2_HMAC(password.c_str(), password.length(),
                              reinterpret_cast<const unsigned char*>(salt.c_str()), salt.length(),
                              iterations, EVP_sha256(),
                              keyLength, derivedKey.data()) != 1) {
            throw std::runtime_error("Password hashing failed");
        }
        
        std::stringstream ss;
        for (auto byte : derivedKey) {
            ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte);
        }
        return ss.str();
    }
};

} // namespace utils