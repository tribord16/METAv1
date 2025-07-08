/**
 * @file AuthException.h
 * @brief Exceptions pour l'authentification
 */

#pragma once
#include <stdexcept>
#include <string>

namespace exceptions {

class AuthException : public std::runtime_error {
public:
    explicit AuthException(const std::string& message) 
        : std::runtime_error(message) {}
};

class InvalidCredentialsException : public AuthException {
public:
    InvalidCredentialsException() 
        : AuthException("Invalid credentials") {}
};

class UserNotFoundException : public AuthException {
public:
    UserNotFoundException() 
        : AuthException("User not found") {}
};

class UserAlreadyExistsException : public AuthException {
public:
    explicit UserAlreadyExistsException(const std::string& field)
        : AuthException("User already exists: " + field) {}
};

class InvalidTokenException : public AuthException {
public:
    InvalidTokenException() 
        : AuthException("Invalid or expired token") {}
};

} // namespace exceptions