// ====== ./include/repositories/UserRepository.h ======
/**
 * @file UserRepository.h
 * @brief Repository pour la gestion des utilisateurs - Couche d'accès aux données
 * @author Meta League Backend Team
 * @date 2025
 * @version 1.0
 *
 * RESPONSABILITÉS :
 * - Encapsuler TOUS les accès à la table 'users' de la base de données
 * - Fournir une interface métier simple pour les opérations CRUD utilisateur
 * - Gérer les requêtes SQL asynchrones avec le client Drogon ORM
 * - Transformer les données DB (Row) en entités métier (User model)
 * - Gérer les erreurs de base de données et les transformer en erreurs métier
 *
 * ARCHITECTURE :
 * ```
 * Service Layer → UserRepository → Drogon ORM → PostgreSQL
 *                      ↓
 *               SQL Queries (async)
 *                      ↓
 *               Row → User Model transformation
 * ```
 *
 * PATTERN REPOSITORY :
 * - Abstraction complète de la couche de données
 * - Interface métier claire (findByEmail, existsByUsername, etc.)
 * - Pas de SQL visible dans les couches supérieures
 * - Facilite les tests unitaires (mocking possible)
 * - Réutilisable par plusieurs services
 *
 * GESTION ASYNC :
 * - Toutes les méthodes utilisent des callbacks (compatible Drogon)
 * - Pas de blocage des threads avec execSqlAsync()
 * - Gestion d'erreurs par callbacks séparés
 * - Performance optimale avec pool de connexions
 *
 * BASE DE DONNÉES :
 * ```sql
 * CREATE TABLE users (
 *     id SERIAL PRIMARY KEY,
 *     username VARCHAR(50) UNIQUE NOT NULL,
 *     email VARCHAR(255) UNIQUE NOT NULL,
 *     password_hash VARCHAR(255) NOT NULL,
 *     is_active BOOLEAN DEFAULT TRUE,
 *     created_at TIMESTAMP DEFAULT NOW(),
 *     updated_at TIMESTAMP DEFAULT NOW()
 * );
 * ```
 *
 * EXEMPLE D'UTILISATION :
 * ```cpp
 * auto dbClient = app().getDbClient();
 * auto userRepo = std::make_shared<UserRepository>(dbClient);
 * 
 * // Recherche d'un utilisateur
 * userRepo->findByUsername("alice", [](std::optional<User> user) {
 *     if (user.has_value()) {
 *         std::cout << "User found: " << user->email << std::endl;
 *     } else {
 *         std::cout << "User not found" << std::endl;
 *     }
 * });
 * ```
 */

#pragma once
#include "models/User.h"
#include <drogon/orm/DbClient.h>
#include <functional>
#include <optional>
#include <string>

namespace repositories {

/**
 * @class UserRepository
 * @brief Repository principal pour toutes les opérations sur la table 'users'
 *
 * Cette classe implémente le pattern Repository qui :
 * - Encapsule complètement l'accès aux données utilisateur
 * - Fournit une API métier claire et typée
 * - Gère les requêtes SQL asynchrones avec gestion d'erreurs
 * - Transforme les données DB en objets métier
 * - Masque la complexité ORM aux couches supérieures
 *
 * DESIGN PATTERNS :
 * - Repository Pattern : abstraction de la couche de données
 * - Async/Callback Pattern : compatible avec Drogon
 * - Single Responsibility : uniquement table users
 * - Dependency Injection : DbClient injecté via constructeur
 *
 * PERFORMANCE :
 * - Utilise le pool de connexions Drogon (configuré dans config.json)
 * - Requêtes SQL préparées (protection injection SQL)
 * - Opérations asynchrones non-bloquantes
 * - Index DB sur username et email pour performance
 *
 * THREAD SAFETY :
 * - Thread-safe : Drogon ORM gère la concurrence
 * - Pas d'état mutable dans la classe
 * - Chaque opération est indépendante
 */
class UserRepository {
private:
    /**
     * @brief Client de base de données Drogon ORM
     * 
     * Instance partagée du pool de connexions PostgreSQL.
     * Configuré dans config.json avec :
     * - Nombre de connexions du pool
     * - Timeout des requêtes
     * - Credentials de connexion
     */
    drogon::orm::DbClientPtr db_;

public:
    /**
     * @brief Constructeur du repository utilisateur
     * 
     * Initialise le repository avec un client de base de données.
     * Le client est généralement obtenu via app().getDbClient().
     * 
     * @param db Client de base de données Drogon (pool de connexions)
     * 
     * EXEMPLE D'INSTANCIATION :
     * ```cpp
     * // Dans un service ou contrôleur
     * auto dbClient = app().getDbClient("default"); // nom du client dans config.json
     * auto userRepo = std::make_shared<UserRepository>(dbClient);
     * ```
     * 
     * @note Le client DB doit être configuré et connecté avant utilisation
     * @note Thread-safe : le client Drogon gère la concurrence
     */
    explicit UserRepository(drogon::orm::DbClientPtr db);

    // ==================== OPÉRATIONS CRUD ====================

    /**
     * @brief Crée un nouvel utilisateur en base de données
     * 
     * PROCESSUS COMPLET DE CRÉATION :
     * 
     * 1. **INSERTION SQL**
     *    - INSERT avec tous les champs utilisateur
     *    - Génération automatique de l'ID (SERIAL)
     *    - Timestamps automatiques (created_at, updated_at)
     * 
     * 2. **GESTION DES CONTRAINTES**
     *    - Contrainte UNIQUE sur username → erreur spécifique
     *    - Contrainte UNIQUE sur email → erreur spécifique
     *    - Autres erreurs DB → erreur générique
     * 
     * 3. **RETOUR DE L'UTILISATEUR CRÉÉ**
     *    - Récupération de l'utilisateur avec son ID généré
     *    - Callback de succès avec l'entité complète
     * 
     * @param user Entité User à insérer (sans ID, sera généré)
     * @param onSuccess Callback appelé avec l'utilisateur créé (avec ID)
     * @param onError Callback appelé avec message d'erreur explicite
     * 
     * EXEMPLE D'UTILISATION :
     * ```cpp
     * User newUser;
     * newUser.username = "alice_martin";
     * newUser.email = "alice@example.com";
     * newUser.password_hash = "$2b$12$hashed_password...";
     * newUser.is_active = true;
     * 
     * userRepo->create(newUser,
     *     [](const User& createdUser) {
     *         LOG("User created with ID: " + std::to_string(createdUser.id));
     *         // Continuer le traitement (générer JWT, etc.)
     *     },
     *     [](const std::string& error) {
     *         LOG("Creation failed: " + error);
     *         // Gérer l'erreur (afficher à l'utilisateur, etc.)
     *     }
     * );
     * ```
     * 
     * CALLBACKS DE SUCCÈS :
     * ```cpp
     * onSuccess(User {
     *     id = 42,                    // ← Généré par la DB
     *     username = "alice_martin",
     *     email = "alice@example.com",
     *     password_hash = "$2b$12$...",
     *     is_active = true,
     *     created_at = "2025-07-08 14:30:15",
     *     updated_at = "2025-07-08 14:30:15"
     * });
     * ```
     * 
     * CALLBACKS D'ERREUR :
     * ```cpp
     * onError("Username already exists"); // Si contrainte username
     * onError("Email already exists");    // Si contrainte email
     * onError("Database error");          // Autre erreur DB
     * ```
     * 
     * REQUÊTE SQL GÉNÉRÉE :
     * ```sql
     * INSERT INTO users (username, email, password_hash, is_active, created_at, updated_at) 
     * VALUES (?, ?, ?, ?, NOW(), NOW())
     * ```
     * 
     * @note Async : n'interrompt jamais le thread appelant
     * @note Thread-safe : peut être appelé depuis plusieurs threads
     * @note Transactionnel : l'insertion est atomique
     */
    void create(const models::User& user,
                std::function<void(const models::User&)> onSuccess,
                std::function<void(const std::string&)> onError);

    /**
     * @brief Recherche un utilisateur actif par son nom d'utilisateur
     * 
     * SPÉCIFICITÉS DE LA RECHERCHE :
     * - Recherche UNIQUEMENT les utilisateurs actifs (is_active = TRUE)
     * - Sensible à la casse (username exact)
     * - Utilise l'index unique sur username pour performance optimale
     * 
     * @param username Nom d'utilisateur à rechercher (exact, sensible à la casse)
     * @param callback Fonction appelée avec le résultat (User ou nullopt)
     * 
     * EXEMPLE D'UTILISATION :
     * ```cpp
     * userRepo->findByUsername("alice_martin", [](std::optional<User> userOpt) {
     *     if (userOpt.has_value()) {
     *         User user = userOpt.value();
     *         LOG("User found: " + user.email);
     *         // Procéder à l'authentification
     *     } else {
     *         LOG("User not found or inactive");
     *         // Retourner erreur "credentials invalides"
     *     }
     * });
     * ```
     * 
     * REQUÊTE SQL :
     * ```sql
     * SELECT * FROM users WHERE username = ? AND is_active = TRUE
     * ```
     * 
     * CALLBACKS :
     * - `callback(std::optional<User>{user})` si trouvé
     * - `callback(std::nullopt)` si non trouvé, inactif, ou erreur DB
     * 
     * @note Performance : utilise l'index unique sur username
     * @note Sécurité : ne retourne que les utilisateurs actifs
     * @note Gestion d'erreur : erreur DB = nullopt (pas d'exception)
     */
    void findByUsername(const std::string& username,
                       std::function<void(std::optional<models::User>)> callback);

    /**
     * @brief Recherche un utilisateur actif par son adresse email
     * 
     * Fonctionnement identique à findByUsername() mais sur le champ email.
     * Permet l'authentification par email au lieu du username si souhaité.
     * 
     * @param email Adresse email à rechercher (exact, insensible à la casse selon DB)
     * @param callback Fonction appelée avec le résultat (User ou nullopt)
     * 
     * EXEMPLE D'UTILISATION :
     * ```cpp
     * userRepo->findByEmail("alice@example.com", [](std::optional<User> userOpt) {
     *     if (userOpt.has_value()) {
     *         // Utilisateur trouvé, procéder à l'auth
     *     } else {
     *         // Email non trouvé ou utilisateur inactif
     *     }
     * });
     * ```
     * 
     * REQUÊTE SQL :
     * ```sql
     * SELECT * FROM users WHERE email = ? AND is_active = TRUE
     * ```
     * 
     * @note PostgreSQL : email généralement insensible à la casse selon collation
     * @note Index unique sur email pour performance
     * @note Ne retourne que les utilisateurs actifs
     */
    void findByEmail(const std::string& email,
                    std::function<void(std::optional<models::User>)> callback);

    /**
     * @brief Recherche un utilisateur actif par son ID
     * 
     * Principalement utilisé pour :
     * - Récupérer l'utilisateur complet après création (avec l'ID généré)
     * - Validation de tokens JWT (vérifier que l'utilisateur existe toujours)
     * - Opérations administratives
     * 
     * @param id Identifiant unique de l'utilisateur
     * @param callback Fonction appelée avec le résultat (User ou nullopt)
     * 
     * EXEMPLE D'UTILISATION :
     * ```cpp
     * int userId = 42; // Extrait d'un token JWT par exemple
     * userRepo->findById(userId, [](std::optional<User> userOpt) {
     *     if (userOpt.has_value()) {
     *         // Utilisateur valide, traitement autorisé
     *     } else {
     *         // Utilisateur supprimé/désactivé, invalider le token
     *     }
     * });
     * ```
     * 
     * REQUÊTE SQL :
     * ```sql
     * SELECT * FROM users WHERE id = ? AND is_active = TRUE
     * ```
     * 
     * @note Clé primaire : performance maximale
     * @note Utilisé pour validation JWT (user_id dans le token)
     * @note Ne retourne que les utilisateurs actifs
     */
    void findById(int id,
                 std::function<void(std::optional<models::User>)> callback);

    // ==================== OPÉRATIONS DE VÉRIFICATION ====================

    /**
     * @brief Vérifie si un nom d'utilisateur existe déjà (actif ou inactif)
     * 
     * Utilisé pendant l'inscription pour valider l'unicité du username
     * AVANT de tenter l'insertion (évite l'exception de contrainte).
     * 
     * DIFFÉRENCE avec findByUsername() :
     * - existsByUsername() : compte TOUS les users (actifs + inactifs)
     * - findByUsername() : retourne UNIQUEMENT les users actifs
     * 
     * @param username Nom d'utilisateur à vérifier
     * @param callback Fonction appelée avec true si existe, false sinon
     * 
     * EXEMPLE D'UTILISATION :
     * ```cpp
     * userRepo->existsByUsername("alice_martin", [](bool exists) {
     *     if (exists) {
     *         LOG("Username déjà pris, choisir un autre");
     *         showError("Ce nom d'utilisateur n'est pas disponible");
     *     } else {
     *         LOG("Username disponible, continuer l'inscription");
     *         proceedWithRegistration();
     *     }
     * });
     * ```
     * 
     * REQUÊTE SQL :
     * ```sql
     * SELECT COUNT(*) as count FROM users WHERE username = ?
     * ```
     * 
     * CALLBACKS :
     * - `callback(true)` si username existe (même si utilisateur inactif)
     * - `callback(false)` si username libre ou erreur DB
     * 
     * @note Compte TOUS les utilisateurs (actifs ET inactifs)
     * @note Utilisé pour validation avant insertion
     * @note Performance : utilise l'index unique sur username
     */
    void existsByUsername(const std::string& username,
                         std::function<void(bool)> callback);

    /**
     * @brief Vérifie si une adresse email existe déjà (actif ou inactif)
     * 
     * Fonctionnement identique à existsByUsername() mais pour l'email.
     * Validation d'unicité pendant l'inscription.
     * 
     * @param email Adresse email à vérifier
     * @param callback Fonction appelée avec true si existe, false sinon
     * 
     * EXEMPLE D'UTILISATION :
     * ```cpp
     * userRepo->existsByEmail("alice@example.com", [](bool exists) {
     *     if (exists) {
     *         showError("Cette adresse email est déjà utilisée");
     *     } else {
     *         continueRegistration();
     *     }
     * });
     * ```
     * 
     * REQUÊTE SQL :
     * ```sql
     * SELECT COUNT(*) as count FROM users WHERE email = ?
     * ```
     * 
     * @note Compte TOUS les utilisateurs (actifs ET inactifs)
     * @note Évite les tentatives d'insertion avec email dupliqué
     * @note Performance : utilise l'index unique sur email
     */
    void existsByEmail(const std::string& email,
                      std::function<void(bool)> callback);

private:
    // ==================== MÉTHODES UTILITAIRES PRIVÉES ====================

    /**
     * @brief Transforme une ligne de résultat DB en entité User
     * 
     * Cette méthode utilitaire convertit les données brutes de la base
     * (drogon::orm::Row) en objet métier typé (models::User).
     * 
     * TRANSFORMATION EFFECTUÉE :
     * - Extraction et conversion des types SQL vers C++
     * - Gestion des valeurs NULL potentielles
     * - Conversion des timestamps en std::chrono::time_point
     * - Validation basique des données extraites
     * 
     * @param row Ligne de résultat de requête SQL
     * @return models::User Entité User complètement hydratée
     * 
     * EXEMPLE DE TRANSFORMATION :
     * ```cpp
     * // Ligne SQL brute :
     * Row { "id": 42, "username": "alice", "email": "alice@ex.com", ... }
     * 
     * // Devient :
     * User {
     *     id = 42,
     *     username = "alice",
     *     email = "alice@ex.com",
     *     password_hash = "$2b$12$...",
     *     is_active = true,
     *     created_at = time_point{...},
     *     updated_at = time_point{...}
     * }
     * ```
     * 
     * GESTION DES TYPES :
     * - id : int (SERIAL/INTEGER)
     * - strings : std::string (VARCHAR/TEXT)
     * - is_active : bool (BOOLEAN)
     * - timestamps : std::chrono::system_clock::time_point (TIMESTAMP)
     * 
     * @note Méthode privée : utilisée uniquement en interne
     * @note Pas de validation métier (fait par les models)
     * @note Exception possible si structure DB incompatible
     */
    models::User rowToUser(const drogon::orm::Row& row);
};

} // namespace repositories

