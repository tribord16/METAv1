#include "dto/corporation/CorporationResponse.h"
#include <chrono>

namespace dto {
namespace corporation {

CorporationResponse CorporationResponse::fromModel(const Corporations& corp) {
    CorporationResponse resp;
    resp.id = corp.getValueOfId();
    resp.name = corp.getValueOfName();
    resp.budget = std::stod(corp.getValueOfBudget()); // budget est string dans DB
    resp.owner_id = corp.getValueOfUserId();
    
    // Conversion des dates trantor::Date vers time_t
    resp.created_at = corp.getValueOfCreatedAt().secondsSinceEpoch();
    resp.updated_at = corp.getValueOfLastPlayed().secondsSinceEpoch();
    
    resp.esports_active = corp.getValueOfEsportsActive();
    resp.racing_active = corp.getValueOfRacingActive();
    resp.tactical_active = corp.getValueOfTacticalActive();
    resp.innovation_active = corp.getValueOfInnovationActive();
    resp.is_success = true;
    resp.message = "Corporation created successfully";
    return resp;
}

CorporationResponse CorporationResponse::error(const std::string& msg) {
    CorporationResponse resp;
    resp.is_success = false;
    resp.message = msg;
    return resp;
}

Json::Value CorporationResponse::toJson() const {
    Json::Value json;
    json["id"] = id;
    json["name"] = name;
    json["budget"] = budget;
    json["owner_id"] = owner_id;
    json["created_at"] = created_at;
    json["updated_at"] = updated_at;
    json["esports_active"] = esports_active;
    json["racing_active"] = racing_active;
    json["tactical_active"] = tactical_active;
    json["innovation_active"] = innovation_active;
    json["is_success"] = is_success;
    json["message"] = message;
    return json;
}

} // namespace corporation
} // namespace dto