// ====== ./include/utils/Logger.h ======
/**
 * @file Logger.h
 * @brief Système de logging applicatif thread-safe
 * @author Meta League Backend Team
 * @date 2025
 * @version 1.0
 *
 * RESPONSABILITÉS :
 * - Fournir un logging uniforme pour toute l'application
 * - Gérer différents niveaux de log (INFO, ERROR, DEBUG, WARN)
 * - Assurer la thread-safety pour utilisation multi-thread
 * - Formater les messages avec timestamps
 * - Compléter le logging Drogon avec du logging applicatif
 *
 * DIFFÉRENCE AVEC DROGON LOGGING :
 * - Drogon : logs framework (requêtes HTTP, erreurs système)
 * - Notre Logger : logs métier (création utilisateur, authentification, etc.)
 * - Complémentaires, pas en remplacement
 *
 * NIVEAUX DE LOG :
 * - INFO : événements normaux (création user, login success)
 * - ERROR : erreurs métier ou techniques (DB error, auth failed)
 * - DEBUG : détails pour développement (SQL queries, validation steps)
 * - WARN : situations anormales mais non critiques (token expired)
 *
 * THREAD SAFETY :
 * - Utilisation de std::mutex pour synchronisation
 * - Safe pour utilisation depuis plusieurs threads simultanément
 * - Compatible avec l'architecture asynchrone de Drogon
 *
 * EXEMPLE D'UTILISATION :
 * ```cpp
 * #include "utils/Logger.h"
 * 
 * // Dans un service
 * Logger::info("User registration started for: " + username);
 * Logger::error("Database connection failed: " + error);
 * Logger::debug("SQL query: " + sql);
 * Logger::warn("Deprecated API endpoint called");
 * ```
 */

#pragma once
#include <string>
#include <mutex>
#include <iostream>
#include <sstream>
#include <chrono>
#include <iomanip>

/**
 * @class Logger
 * @brief Système de logging statique thread-safe pour l'application
 *
 * Cette classe fournit des méthodes statiques pour logger des événements
 * avec différents niveaux de criticité. Tous les logs incluent automatiquement :
 * - Timestamp précis (date + heure)
 * - Niveau de log ([INFO], [ERROR], etc.)
 * - Message applicatif
 *
 * DESIGN PATTERNS :
 * - Singleton implicite (méthodes statiques)
 * - Thread-safe avec mutex global
 * - Simple à utiliser (pas d'instanciation)
 * - Extensible (facile d'ajouter sortie fichier, etc.)
 *
 * FORMAT DE SORTIE :
 * ```
 * [2025-07-08 14:30:15] [INFO] User created successfully: alice_martin
 * [2025-07-08 14:30:16] [ERROR] Database connection failed
 * [2025-07-08 14:30:17] [DEBUG] Processing JWT token validation
 * [2025-07-08 14:30:18] [WARN] Rate limit approaching for IP: 192.168.1.10
 * ```
 *
 * UTILISATION RECOMMANDÉE :
 * - INFO : événements métier importants
 * - ERROR : erreurs à investiguer
 * - DEBUG : détails pour développement (désactiver en prod)
 * - WARN : situations à surveiller
 */
class Logger {
public:
    /**
     * @enum Level
     * @brief Niveaux de criticité des logs
     * 
     * Hiérarchie de criticité (du moins au plus critique) :
     * DEBUG < INFO < WARN < ERROR
     */
    enum class Level { 
        DEBUG,  ///< Détails de développement (désactiver en production)
        INFO,   ///< Événements normaux et informatifs  
        WARN,   ///< Situations anormales mais non critiques
        ERROR   ///< Erreurs nécessitant une investigation
    };

    // ==================== MÉTHODES PUBLIQUES (INTERFACE SIMPLE) ====================

    /**
     * @brief Log un message d'information
     * @param msg Message à logger
     * 
     * Utilisé pour les événements normaux et importants :
     * - Démarrage de l'application
     * - Création/modification d'entités
     * - Succès d'opérations métier
     * 
     * EXEMPLES :
     * ```cpp
     * Logger::info("Application started successfully");
     * Logger::info("User registered: " + user.username);
     * Logger::info("JWT token generated for user ID: " + std::to_string(userId));
     * ```
     */
    static void info(const std::string& msg);

    /**
     * @brief Log un message d'erreur
     * @param msg Message d'erreur à logger
     * 
     * Utilisé pour les erreurs techniques ou métier :
     * - Erreurs de base de données
     * - Échecs d'authentification
     * - Exceptions non gérées
     * - Erreurs de validation critique
     * 
     * EXEMPLES :
     * ```cpp
     * Logger::error("Database connection failed: " + dbError);
     * Logger::error("JWT validation failed for token: " + token);
     * Logger::error("User creation failed: " + validationError);
     * ```
     */
    static void error(const std::string& msg);

    /**
     * @brief Log un message de debug
     * @param msg Message de debug à logger
     * 
     * Utilisé pour les détails de développement :
     * - Étapes de traitement
     * - Valeurs de variables
     * - Flux d'exécution
     * - Requêtes SQL
     * 
     * ⚠️ À désactiver en production pour performance
     * 
     * EXEMPLES :
     * ```cpp
     * Logger::debug("Processing registration request for: " + username);
     * Logger::debug("SQL query: " + sqlQuery);
     * Logger::debug("JWT payload extracted: user_id=" + std::to_string(userId));
     * ```
     */
    static void debug(const std::string& msg);

    /**
     * @brief Log un message d'avertissement
     * @param msg Message d'avertissement à logger
     * 
     * Utilisé pour les situations anormales mais non critiques :
     * - Tentatives d'authentification échouées
     * - Utilisation d'APIs dépréciées
     * - Limites de rate limiting approchées
     * - Configurations non optimales
     * 
     * EXEMPLES :
     * ```cpp
     * Logger::warn("Failed login attempt for user: " + username);
     * Logger::warn("API endpoint deprecated: " + endpoint);
     * Logger::warn("Rate limit warning for IP: " + clientIP);
     * ```
     */
    static void warn(const std::string& msg);

    /**
     * @brief Méthode générique de logging avec niveau spécifique
     * @param level Niveau de criticité du message
     * @param msg Message à logger
     * 
     * Méthode de base utilisée par toutes les autres.
     * Peut être utilisée directement si besoin de niveaux dynamiques.
     * 
     * EXEMPLE :
     * ```cpp
     * Logger::Level currentLevel = getConfiguredLogLevel();
     * Logger::log(currentLevel, "Dynamic level message");
     * ```
     */
    static void log(Level level, const std::string& msg);

private:
    // ==================== MEMBRES PRIVÉS ====================

    /**
     * @brief Mutex pour synchronisation thread-safe
     * 
     * Assure qu'un seul thread peut écrire dans la console
     * à la fois, évitant les messages entremêlés.
     */
    static std::mutex logMutex;

    /**
     * @brief Convertit un niveau en string pour affichage
     * @param level Niveau à convertir
     * @return std::string Représentation textuelle ("[INFO]", "[ERROR]", etc.)
     */
    static std::string levelToString(Level level);

    /**
     * @brief Génère le timestamp actuel formaté
     * @return std::string Timestamp au format "YYYY-MM-DD HH:MM:SS"
     * 
     * Utilisé pour préfixer tous les messages de log.
     * Format compatible avec la plupart des outils d'analyse de logs.
     */
    static std::string currentDateTime();
};