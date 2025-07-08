// src/config/AppConfig.cpp
#include "config/AppConfig.h"
#include <fstream>
#include <map>
#include <stdexcept>

static std::map<std::string, std::string> vars;

void AppConfig::init() {
    std::ifstream file("../config/dev.env");
    if (!file.is_open()) throw std::runtime_error("Config file not found");
    
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            vars[line.substr(0, pos)] = line.substr(pos + 1);
        }
    }
}

std::string AppConfig::get(const std::string& key) { return vars[key]; }
double AppConfig::getDouble(const std::string& key) { return std::stod(vars[key]); }
bool AppConfig::getBool(const std::string& key) { return vars[key] == "true"; }
int AppConfig::getInt(const std::string& key) { return std::stoi(vars[key]); }

// Getters
std::string AppConfig::getAppName() { return get("APP_NAME"); }
bool AppConfig::isDebug() { return getBool("DEBUG_MODE"); }

std::string AppConfig::getServerHost() { return get("SERVER_HOST"); }
int AppConfig::getServerPort() { return getInt("SERVER_PORT"); }

std::string AppConfig::getDbHost() { return get("DB_HOST"); }
int AppConfig::getDbPort() { return getInt("DB_PORT"); }
std::string AppConfig::getDbName() { return get("DB_NAME"); }
std::string AppConfig::getDbUser() { return get("DB_USER"); }
std::string AppConfig::getDbPassword() { return get("DB_PASSWORD"); }

std::string AppConfig::getJwtSecret() { return get("JWT_SECRET"); }

double AppConfig::getDefaultBudget() { return getDouble("GAME_DEFAULT_CORPORATION_BUDGET"); }
double AppConfig::getMinSalary() { return getDouble("GAME_MINIMUM_SALARY"); }
double AppConfig::getMaxSalary() { return getDouble("GAME_MAXIMUM_SALARY"); }
double AppConfig::getDefaultPrize() { return getDouble("GAME_DEFAULT_PRIZE_MONEY"); }