// ====== ./include/utils/CryptoUtils.h ======
/**
 * @file CryptoUtils.h
 * @brief Utilitaires cryptographiques pour sécurité des mots de passe
 * @author Meta League Backend Team
 * @date 2025
 * @version 1.0
 *
 * RESPONSABILITÉS :
 * - Hash sécurisé des mots de passe avec BCrypt
 * - Vérification des mots de passe contre leurs hashs
 * - Génération de clés aléatoires sécurisées
 * - Encapsuler toute la cryptographie de l'application
 *
 * SÉCURITÉ IMPLÉMENTÉE :
 * - BCrypt avec salt aléatoire (protection contre rainbow tables)
 * - Cost factor 12 (équilibre sécurité/performance)
 * - Génération aléatoire cryptographiquement sûre
 * - Pas de stockage de mots de passe en clair JAMAIS
 *
 * ALGORITHMES :
 * - Hash : BCrypt avec salt automatique
 * - Random : std::random_device + std::mt19937
 * - Vérification : comparaison temps constant (protection timing attacks)
 *
 * EXEMPLE D'UTILISATION :
 * ```cpp
 * // Lors de l'inscription
 * std::string password = "motDePasseUtilisateur";
 * std::string hash = CryptoUtils::hashPassword(password);
 * user.password_hash = hash; // Stocker le hash, jamais le password
 * 
 * // Lors de la connexion
 * bool isValid = CryptoUtils::verifyPassword(password, user.password_hash);
 * if (isValid) {
 *     // Authentification réussie
 * }
 * ```
 */

#pragma once
#include <string>

namespace utils {

/**
 * @class CryptoUtils
 * @brief Classe utilitaire statique pour opérations cryptographiques
 *
 * Cette classe fournit toutes les primitives cryptographiques
 * nécessaires à la sécurisation de l'application :
 * - Hachage sécurisé des mots de passe
 * - Vérification des credentials
 * - Génération de secrets aléatoires
 *
 * DESIGN :
 * - Classe statique (pas d'instanciation)
 * - Méthodes thread-safe
 * - Interface simple et sûre
 * - Utilisation de bibliothèques cryptographiques éprouvées
 *
 * SÉCURITÉ :
 * - Utilise BCrypt (standard industrie)
 * - Salt aléatoire automatique
 * - Protection contre timing attacks
 * - Pas de mémorisation de secrets
 */
class CryptoUtils {
public:
    /**
     * @brief Hash un mot de passe avec BCrypt et salt aléatoire
     * 
     * Transforme un mot de passe en clair en hash sécurisé pour stockage.
     * Utilise BCrypt avec un salt aléatoire généré automatiquement.
     * 
     * PROCESSUS :
     * 1. Génération d'un salt aléatoire unique
     * 2. Application de l'algorithme BCrypt avec cost factor 12
     * 3. Retour du hash complet (inclut le salt)
     * 
     * @param password Mot de passe en clair à hasher
     * @return std::string Hash BCrypt complet (salt + hash)
     * 
     * FORMAT DE RETOUR :
     * ```
     * $2b$12$saltaletoirede22chars.hashresultingde31chars
     * ```
     * 
     * EXEMPLE :
     * ```cpp
     * std::string userPassword = "monMotDePasseSecret123";
     * std::string hashedPassword = CryptoUtils::hashPassword(userPassword);
     * 
     * // hashedPassword = "$2b$12$N9qo8uLOickgx2ZMRZoMye.IYvfCa3ux..."
     * 
     * User user;
     * user.password_hash = hashedPassword; // Stocker le hash
     * // JAMAIS stocker userPassword !
     * ```
     * 
     * SÉCURITÉ :
     * - Salt unique pour chaque mot de passe
     * - Cost factor 12 (recommandé 2025)
     * - Résistant aux attaques par dictionnaire
     * - Résistant aux rainbow tables
     * 
     * @note Ne jamais stocker le password original après hachage
     * @note Le hash retourné contient toutes les infos pour vérification
     * @note Thread-safe : peut être appelé depuis plusieurs threads
     */
    static std::string hashPassword(const std::string& password);

    /**
     * @brief Vérifie un mot de passe contre son hash BCrypt
     * 
     * Compare un mot de passe en clair avec un hash BCrypt stocké
     * pour déterminer s'ils correspondent. Utilise une comparaison
     * en temps constant pour éviter les timing attacks.
     * 
     * @param password Mot de passe en clair à vérifier
     * @param hash Hash BCrypt stocké (provenant de hashPassword())
     * @return true si le password correspond au hash, false sinon
     * 
     * EXEMPLE D'AUTHENTIFICATION :
     * ```cpp
     * // Lors de la connexion
     * std::string inputPassword = getPasswordFromLoginForm();
     * std::string storedHash = user.password_hash; // Depuis la DB
     * 
     * bool isAuthenticated = CryptoUtils::verifyPassword(inputPassword, storedHash);
     * 
     * if (isAuthenticated) {
     *     generateJwtToken(user);
     *     Logger::info("Login successful for: " + user.username);
     * } else {
     *     Logger::warn("Login failed for: " + user.username);
     *     return createAuthErrorResponse();
     * }
     * ```
     * 
     * SÉCURITÉ :
     * - Comparaison temps constant (anti timing attack)
     * - Utilise le salt contenu dans le hash
     * - Pas de révélation d'information par les temps de réponse
     * 
     * ROBUSTESSE :
     * - Retourne false si hash malformé ou vide
     * - Gère les erreurs cryptographiques gracieusement
     * - Thread-safe
     * 
     * @note Temps de traitement ~100-200ms (normal pour BCrypt cost 12)
     * @note Ne pas utiliser pour comparer des hashs entre eux
     */
    static bool verifyPassword(const std::string& password, const std::string& hash);

    /**
     * @brief Génère une clé aléatoire cryptographiquement sûre
     * 
     * Crée une chaîne aléatoire de longueur spécifiée, utilisable
     * pour des secrets, tokens, clés JWT, etc.
     * 
     * @param length Longueur désirée de la clé (défaut: 32 caractères)
     * @return std::string Clé aléatoire alphanumériqueC
     * 
     * ALPHABET UTILISÉ :
     * - Lettres majuscules : A-Z
     * - Lettres minuscules : a-z  
     * - Chiffres : 0-9
     * - Total : 62 caractères possibles par position
     * 
     * EXEMPLE D'UTILISATION :
     * ```cpp
     * // Génération clé JWT secrète
     * std::string jwtSecret = CryptoUtils::generateRandomKey(64);
     * 
     * // Génération token de reset password
     * std::string resetToken = CryptoUtils::generateRandomKey(32);
     * 
     * // Génération ID de session
     * std::string sessionId = CryptoUtils::generateRandomKey(16);
     * ```
     * 
     * ENTROPIE :
     * - 32 chars = ~190 bits d'entropie (très sécurisé)
     * - 16 chars = ~95 bits d'entropie (sécurisé)
     * - 8 chars = ~47 bits d'entropie (usage temporaire seulement)
     * 
     * @note Utilise std::random_device pour amorçage cryptographique
     * @note Thread-safe : générateur par thread
     * @note Ne pas utiliser pour passwords utilisateur (utiliser hashPassword)
     */
    static std::string generateRandomKey(size_t length = 32);
};

} // namespace utils