#include "dto/corporation/CreateCorporationRequest.h"
#include <algorithm>
#include <cctype>

namespace dto {
namespace corporation {

CreateCorporationRequest CreateCorporationRequest::fromJson(const Json::Value& json) {
    CreateCorporationRequest request;
    
    if (json.isMember("name") && json["name"].isString()) {
        request.name = json["name"].asString();
    }
    
    if (json.isMember("initial_budget") && json["initial_budget"].isNumeric()) {
        request.initial_budget = json["initial_budget"].asDouble();
    }
    
    request.esports_active = json.get("esports_active", false).asBool();
    request.racing_active = json.get("racing_active", false).asBool();
    request.tactical_active = json.get("tactical_active", false).asBool();
    request.innovation_active = json.get("innovation_active", false).asBool();
    
    return request;
}

bool CreateCorporationRequest::isValid() const {
    return isValidName(name) && 
           isValidBudget(initial_budget) && 
           hasAtLeastOneDivision();
}

std::vector<std::string> CreateCorporationRequest::getErrors() const {
    std::vector<std::string> errors;
    
    if (!isValidName(name)) {
        if (name.empty()) {
            errors.push_back("Le nom de la corporation est requis");
        } else if (name.length() < 3) {
            errors.push_back("Le nom doit contenir au moins 3 caractères");
        } else if (name.length() > 50) {
            errors.push_back("Le nom ne peut pas dépasser 50 caractères");
        } else {
            errors.push_back("Le nom contient des caractères non autorisés");
        }
    }
    
    if (!isValidBudget(initial_budget)) {
        if (initial_budget <= 0) {
            errors.push_back("Le budget initial doit être positif");
        } else if (initial_budget < 100000) {
            errors.push_back("Le budget initial minimum est de 100,000€");
        } else if (initial_budget > 10000000) {
            errors.push_back("Le budget initial maximum est de 10,000,000€");
        }
    }
    
    if (!hasAtLeastOneDivision()) {
        errors.push_back("Au moins une division doit être activée");
    }
    
    return errors;
}

bool CreateCorporationRequest::isValidName(const std::string& name) const {
    if (name.empty() || name.length() < 3 || name.length() > 50) {
        return false;
    }
    
    // Vérifier que le nom contient uniquement des caractères alphanumériques, espaces, tirets et underscores
    return std::all_of(name.begin(), name.end(), [](char c) {
        return std::isalnum(c) || c == ' ' || c == '-' || c == '_';
    });
}

bool CreateCorporationRequest::isValidBudget(double budget) const {
    return budget >= 100000.0 && budget <= 10000000.0;
}

bool CreateCorporationRequest::hasAtLeastOneDivision() const {
    return esports_active || racing_active || tactical_active || innovation_active;
}

} // namespace corporation
} // namespace dto
