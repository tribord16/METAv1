// ====== ./include/config/AppConfig.h ======
/**
 * @file AppConfig.h  
 * @brief Gestionnaire de configuration applicative centralisé
 * @author Meta League Backend Team
 * @date 2025
 * @version 1.0
 *
 * RESPONSABILITÉS :
 * - Charger et parser les variables d'environnement (.env)
 * - Fournir une interface typée pour accéder aux configurations
 * - Centraliser TOUTE la configuration applicative
 * - Valider les configurations au démarrage
 * - Séparer config applicative de config Drogon
 *
 * ARCHITECTURE DE CONFIGURATION :
 * ```
 * dev.env (variables) → AppConfig::init() → Getters typés
 * config.json (Drogon) → app().loadConfigFile() → Framework
 * ```
 *
 * SÉPARATION DES RESPONSABILITÉS :
 * - AppConfig : variables métier (DB, JWT, paramètres jeu)
 * - config.json : config framework (serveur, middlewares, ORM)
 *
 * VARIABLES GÉRÉES :
 * - Application : nom, mode debug
 * - Serveur : host, port d'écoute
 * - Base de données : connexion PostgreSQL
 * - JWT : clé secrète pour signature
 * - Jeu : paramètres métier (budgets, salaires, etc.)
 *
 * EXEMPLE DE FICHIER .env :
 * ```
 * APP_NAME=Meta League Backend
 * DEBUG_MODE=true
 * SERVER_HOST=0.0.0.0
 * SERVER_PORT=8080
 * DB_HOST=localhost
 * DB_PORT=5432
 * JWT_SECRET=your-secret-key-here
 * GAME_DEFAULT_CORPORATION_BUDGET=100000.0
 * ```
 *
 * EXEMPLE D'UTILISATION :
 * ```cpp
 * // Au démarrage (main.cpp)
 * AppConfig::init(); // Charge le fichier .env
 * 
 * // Partout dans l'app
 * std::string dbHost = AppConfig::getDbHost();
 * int port = AppConfig::getServerPort();
 * bool isDebug = AppConfig::isDebug();
 * ```
 */

#pragma once
#include <string>

/**
 * @class AppConfig
 * @brief Gestionnaire statique de configuration applicative
 *
 * Cette classe fournit une interface centralisée pour accéder
 * à toutes les configurations de l'application chargées depuis
 * les variables d'environnement.
 *
 * DESIGN PATTERNS :
 * - Singleton statique : une seule instance de config
 * - Lazy initialization : chargement au premier appel init()
 * - Type-safe getters : conversion automatique des types
 * - Fail-fast : erreur immédiate si config manquante
 *
 * AVANTAGES :
 * - Configuration centralisée et typée
 * - Facile à modifier sans recompilation
 * - Séparation environnements (dev/staging/prod)
 * - Validation au démarrage
 */
class AppConfig {
public:
    /**
     * @brief Initialise la configuration depuis le fichier .env
     * 
     * Charge et parse le fichier ../config/dev.env (ou prod.env selon envir).
     * Doit être appelé UNE FOIS au démarrage avant tout autre getter.
     * 
     * PROCESSUS :
     * 1. Ouverture du fichier de configuration
     * 2. Parsing ligne par ligne (KEY=VALUE)
     * 3. Stockage en mémoire dans map interne
     * 4. Validation des variables critiques
     * 
     * @throws std::runtime_error Si fichier introuvable ou malformé
     * 
     * EXEMPLE :
     * ```cpp
     * // Dans main.cpp
     * try {
     *     AppConfig::init(); // Charge ../config/dev.env
     *     Logger::info("Configuration loaded: " + AppConfig::getAppName());
     * } catch (const std::runtime_error& e) {
     *     std::cerr << "Config error: " << e.what() << std::endl;
     *     return 1;
     * }
     * ```
     * 
     * FICHIER .env ATTENDU :
     * ```
     * APP_NAME=Meta League Backend
     * DEBUG_MODE=true
     * SERVER_HOST=0.0.0.0
     * SERVER_PORT=8080
     * DB_HOST=localhost
     * DB_PORT=5432
     * DB_NAME=meta_league
     * DB_USER=postgres
     * DB_PASSWORD=password
     * JWT_SECRET=your-super-secret-key-here
     * GAME_DEFAULT_CORPORATION_BUDGET=100000.0
     * GAME_MINIMUM_SALARY=30000.0
     * GAME_MAXIMUM_SALARY=500000.0
     * GAME_DEFAULT_PRIZE_MONEY=50000.0
     * ```
     * 
     * @note Appelé UNE SEULE FOIS au démarrage
     * @note Thread-safe après initialisation
     * @note Fail-fast : erreur immédiate si problème de config
     */
    static void init();
    
    // ==================== GETTERS CONFIGURATION APPLICATION ====================
    
    /**
     * @brief Nom de l'application
     * @return std::string Nom configuré (ex: "Meta League Backend")
     */
    static std::string getAppName();
    
    /**
     * @brief Mode debug activé ou non
     * @return bool true si mode debug, false si production
     * 
     * UTILISATION :
     * ```cpp
     * if (AppConfig::isDebug()) {
     *     Logger::debug("Debug info: " + detailedInfo);
     *     app().setLogLevel(trantor::Logger::kDebug);
     * } else {
     *     app().setLogLevel(trantor::Logger::kInfo);
     * }
     * ```
     */
    static bool isDebug();
    
    // ==================== GETTERS CONFIGURATION SERVEUR ====================
    
    /**
     * @brief Host d'écoute du serveur HTTP
     * @return std::string Adresse IP (ex: "0.0.0.0", "127.0.0.1")
     */
    static std::string getServerHost();
    
    /**
     * @brief Port d'écoute du serveur HTTP  
     * @return int Numéro de port (ex: 8080)
     */
    static int getServerPort();
    
    // ==================== GETTERS CONFIGURATION BASE DE DONNÉES ====================
    
    /**
     * @brief Host de la base de données PostgreSQL
     * @return std::string Adresse du serveur DB (ex: "localhost", "192.168.1.10")
     */
    static std::string getDbHost();
    
    /**
     * @brief Port de la base de données PostgreSQL
     * @return int Port de connexion (généralement 5432)
     */
    static int getDbPort();
    
    /**
     * @brief Nom de la base de données
     * @return std::string Nom de la DB (ex: "meta_league")
     */
    static std::string getDbName();
    
    /**
     * @brief Utilisateur de connexion à la base
     * @return std::string Username DB (ex: "postgres")
     */
    static std::string getDbUser();
    
    /**
     * @brief Mot de passe de connexion à la base
     * @return std::string Password DB
     * 
     * @note En production, utiliser des secrets sécurisés (pas de .env)
     */
    static std::string getDbPassword();
    
    // ==================== GETTERS CONFIGURATION JWT ====================
    
    /**
     * @brief Clé secrète pour signature des tokens JWT
     * @return std::string Clé secrète (doit être complexe et unique par environnement)
     * 
     * SÉCURITÉ :
     * - Utiliser une clé différente par environnement (dev/staging/prod)
     * - Longueur recommandée : 64+ caractères aléatoires
     * - Ne jamais exposer dans les logs ou erreurs
     * - Rotation régulière en production
     * 
     * EXEMPLE DE GÉNÉRATION :
     * ```bash
     * openssl rand -base64 64
     * ```
     */
    static std::string getJwtSecret();
    
    // ==================== GETTERS CONFIGURATION JEU (MÉTIER) ====================
    
    /**
     * @brief Budget par défaut d'une corporation
     * @return double Montant en euros (ex: 100000.0)
     */
    static double getDefaultBudget();
    
    /**
     * @brief Salaire minimum autorisé
     * @return double Montant en euros (ex: 30000.0)
     */
    static double getMinSalary();
    
    /**
     * @brief Salaire maximum autorisé
     * @return double Montant en euros (ex: 500000.0)
     */
    static double getMaxSalary();
    
    /**
     * @brief Prize money par défaut pour les compétitions
     * @return double Montant en euros (ex: 50000.0)
     */
    static double getDefaultPrize();
    
private:
    // ==================== MÉTHODES UTILITAIRES PRIVÉES ====================
    
    /**
     * @brief Récupère une variable d'environnement en string
     * @param key Nom de la variable
     * @return std::string Valeur de la variable
     * @throws std::runtime_error Si variable non trouvée
     */
    static std::string get(const std::string& key);
    
    /**
     * @brief Récupère une variable et la convertit en double
     * @param key Nom de la variable
     * @return double Valeur convertie
     * @throws std::runtime_error Si variable non trouvée ou conversion impossible
     */
    static double getDouble(const std::string& key);
    
    /**
     * @brief Récupère une variable et la convertit en bool
     * @param key Nom de la variable
     * @return bool true si "true", false si "false"
     * @throws std::runtime_error Si variable non trouvée ou valeur invalide
     */
    static bool getBool(const std::string& key);
    
    /**
     * @brief Récupère une variable et la convertit en int
     * @param key Nom de la variable
     * @return int Valeur convertie
     * @throws std::runtime_error Si variable non trouvée ou conversion impossible
     */
    static int getInt(const std::string& key);
};