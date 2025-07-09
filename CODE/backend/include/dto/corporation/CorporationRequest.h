// ============================================================================
// include/dto/corporation/CorporationResponse.h - Corporation Response DTO
// ============================================================================

#pragma once
#include <nlohmann/json.hpp>
#include "models/Corporation.h"

namespace dto {
namespace corporation {

struct CorporationResponse {
    models::Corporation corporation;
    int total_players = 0;
    double monthly_expenses = 0.0;
    double projected_income = 0.0;

    static CorporationResponse fromModel(const models::Corporation& corp) {
        CorporationResponse response;
        response.corporation = corp;
        return response;
    }

    nlohmann::json toJson() const {
        auto json = corporation.toJson();
        json["total_players"] = total_players;
        json["monthly_expenses"] = monthly_expenses;
        json["projected_income"] = projected_income;
        return json;
    }
};

} // namespace corporation
} // namespace dto