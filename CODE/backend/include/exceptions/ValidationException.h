/**
 * @file ValidationException.h
 * @brief Exceptions pour la validation
 */

#pragma once
#include <stdexcept>
#include <string>
#include <vector>

namespace exceptions {

class ValidationException : public std::runtime_error {
private:
    std::vector<std::string> errors_;

public:
    explicit ValidationException(const std::string& message)
        : std::runtime_error(message) {
        errors_.push_back(message);
    }
    
    explicit ValidationException(const std::vector<std::string>& errors)
        : std::runtime_error("Validation failed"), errors_(errors) {}
    
    const std::vector<std::string>& getErrors() const {
        return errors_;
    }
};

} // namespace exceptions