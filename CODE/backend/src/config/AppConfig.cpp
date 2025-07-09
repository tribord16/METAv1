
/************************************************************
 * @file AppConfig.cpp
 * @brief Implémentation du gestionnaire de configuration applicative
 *
 * Rôle :
 *   - Charger et parser le fichier .env (variables d'environnement)
 *   - Fournir des getters typés pour accéder à la configuration
 *
 * Place dans l'architecture :
 *   - Utilisé par tous les modules nécessitant une config (main, services, etc.)
 *
 * Dépendances :
 *   - <fstream>, <map>, <stdexcept>
 *   - config/AppConfig.h
 *
 * TODO :
 *   - Ajouter la gestion multi-environnement (dev/prod)
 *   - Logger les erreurs de parsing et de config manquante
 *   - Ajouter des tests unitaires sur le parsing
 *   - Sécuriser l'accès aux secrets (ne pas logger les valeurs sensibles)
 ************************************************************/

#include "config/AppConfig.h"
#include <fstream>
#include <map>
#include <stdexcept>

// Stockage interne des variables d'environnement chargées
static std::map<std::string, std::string> vars;

/**
 * @brief Initialise la configuration depuis le fichier .env
 *
 * Ouvre et parse le fichier ../config/dev.env (ou prod.env selon l'environnement).
 * Stocke chaque variable dans la map interne vars.
 *
 * @throws std::runtime_error si le fichier est introuvable ou malformé
 *
 * TODO :
 *   - Ajouter la gestion de plusieurs environnements (dev/prod)
 *   - Logger les erreurs de parsing
 *   - Valider la présence des variables critiques
 */
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
        // TODO: Logger les lignes ignorées ou malformées
    }
    // TODO: Valider la présence des variables critiques (APP_NAME, DB_HOST, etc.)
}

/**
 * @brief Récupère la valeur d'une variable d'environnement (string)
 * @param key Nom de la variable
 * @return Valeur string
 * @throws std::out_of_range si la clé n'existe pas
 */
std::string AppConfig::get(const std::string& key) { return vars[key]; }

/**
 * @brief Récupère la valeur d'une variable d'environnement (double)
 * @param key Nom de la variable
 * @return Valeur double
 * @throws std::invalid_argument si conversion impossible
 */
double AppConfig::getDouble(const std::string& key) { return std::stod(vars[key]); }

/**
 * @brief Récupère la valeur d'une variable d'environnement (booléen)
 * @param key Nom de la variable
 * @return true si "true", false sinon
 */
bool AppConfig::getBool(const std::string& key) { return vars[key] == "true"; }

/**
 * @brief Récupère la valeur d'une variable d'environnement (int)
 * @param key Nom de la variable
 * @return Valeur int
 * @throws std::invalid_argument si conversion impossible
 */
int AppConfig::getInt(const std::string& key) { return std::stoi(vars[key]); }

// ==================== GETTERS ====================

/** @brief Nom de l'application */
std::string AppConfig::getAppName() { return get("APP_NAME"); }
/** @brief Mode debug activé ? */
bool AppConfig::isDebug() { return getBool("DEBUG_MODE"); }

/** @brief Host d'écoute du serveur */
std::string AppConfig::getServerHost() { return get("SERVER_HOST"); }
/** @brief Port d'écoute du serveur */
int AppConfig::getServerPort() { return getInt("SERVER_PORT"); }

/** @brief Host de la base de données */
std::string AppConfig::getDbHost() { return get("DB_HOST"); }
/** @brief Port de la base de données */
int AppConfig::getDbPort() { return getInt("DB_PORT"); }
/** @brief Nom de la base de données */
std::string AppConfig::getDbName() { return get("DB_NAME"); }
/** @brief Utilisateur DB */
std::string AppConfig::getDbUser() { return get("DB_USER"); }
/** @brief Mot de passe DB */
std::string AppConfig::getDbPassword() { return get("DB_PASSWORD"); }

/** @brief Secret JWT */
std::string AppConfig::getJwtSecret() { return get("JWT_SECRET"); }

/** @brief Budget par défaut d'une corporation */
double AppConfig::getDefaultBudget() { return getDouble("GAME_DEFAULT_CORPORATION_BUDGET"); }
/** @brief Salaire minimum */
double AppConfig::getMinSalary() { return getDouble("GAME_MINIMUM_SALARY"); }
/** @brief Salaire maximum */
double AppConfig::getMaxSalary() { return getDouble("GAME_MAXIMUM_SALARY"); }
/** @brief Prime par défaut */
double AppConfig::getDefaultPrize() { return getDouble("GAME_DEFAULT_PRIZE_MONEY"); }