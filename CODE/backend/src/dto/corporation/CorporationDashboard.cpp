#include "dto/corporation/CorporationDashboard.h"

namespace dto {
namespace corporation {

Json::Value FinancialSummary::toJson() const {
    Json::Value json;
    json["current_budget"] = current_budget;
    json["monthly_income"] = monthly_income;
    json["monthly_expenses"] = monthly_expenses;
    json["net_monthly"] = net_monthly;
    json["projected_budget_3m"] = projected_budget_3m;
    return json;
}

Json::Value TeamSummary::toJson() const {
    Json::Value json;
    json["total_players"] = total_players;
    json["average_rating"] = average_rating;
    json["total_salary"] = total_salary;
    json["team_chemistry"] = team_chemistry;
    
    Json::Value missing(Json::arrayValue);
    for (const auto& position : missing_positions) {
        missing.append(position);
    }
    json["missing_positions"] = missing;
    
    return json;
}

Json::Value UpcomingEvents::toJson() const {
    Json::Value json;
    
    Json::Value matches(Json::arrayValue);
    for (const auto& match : next_matches) {
        matches.append(match);
    }
    json["next_matches"] = matches;
    
    Json::Value deadlinesJson(Json::arrayValue);
    for (const auto& deadline : deadlines) {
        deadlinesJson.append(deadline);
    }
    json["deadlines"] = deadlinesJson;
    
    Json::Value opportunitiesJson(Json::arrayValue);
    for (const auto& opportunity : opportunities) {
        opportunitiesJson.append(opportunity);
    }
    json["opportunities"] = opportunitiesJson;
    
    return json;
}

Json::Value CorporationDashboard::toJson() const {
    Json::Value json;
    
    // Corporation info
    Json::Value corpInfo;
    corpInfo["id"] = corporation.getValueOfId();
    corpInfo["name"] = corporation.getValueOfName();
    corpInfo["budget"] = corporation.getValueOfBudget();
    corpInfo["reputation"] = corporation.getValueOfReputation();
    corpInfo["current_season"] = corporation.getValueOfCurrentSeason();
    corpInfo["current_week"] = corporation.getValueOfCurrentWeek();
    corpInfo["esports_active"] = corporation.getValueOfEsportsActive();
    corpInfo["racing_active"] = corporation.getValueOfRacingActive();
    corpInfo["tactical_active"] = corporation.getValueOfTacticalActive();
    corpInfo["innovation_active"] = corporation.getValueOfInnovationActive();
    json["corporation"] = corpInfo;
    
    // Sections du dashboard
    json["finances"] = finances.toJson();
    json["team"] = team.toJson();
    json["events"] = events.toJson();
    
    // Meta-information
    json["current_week"] = current_week;
    json["current_phase"] = current_phase;
    
    Json::Value notificationsJson(Json::arrayValue);
    for (const auto& notification : notifications) {
        notificationsJson.append(notification);
    }
    json["notifications"] = notificationsJson;
    
    return json;
}

} // namespace corporation
} // namespace dto
