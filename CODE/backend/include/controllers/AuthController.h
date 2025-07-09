
/************************************************************
 * @file AuthController.h
 * @brief Contrôleur d'authentification (Drogon HttpController)
 *
 * Rôle :
 *   - Gérer les routes d'authentification (register, login, me)
 *   - Orchestrer la logique métier via AuthService
 *   - Protéger les routes sensibles avec middleware JWT
 *
 * Place dans l'architecture :
 *   - Contrôleur HTTP principal pour l'authentification
 *   - Appelé automatiquement par Drogon via la macro PATH_LIST
 *
 * Dépendances :
 *   - Drogon (HttpController)
 *   - services/AuthService (logique métier)
 *   - middlewares/JwtMiddleware (protection JWT)
 *
 * TODO :
 *   - Ajouter des validations plus strictes sur les entrées utilisateur
 *   - Ajouter des tests unitaires sur chaque handler
 *   - Logger les tentatives d'accès non autorisées
 *   - Factoriser la gestion des erreurs
 ************************************************************/

#include <drogon/HttpController.h>
#include "services/AuthService.h"
#include <memory>

using namespace drogon;

namespace controllers {


class AuthController : public drogon::HttpController<AuthController> {
private:
    // Service métier d'authentification (injection de dépendance)
    std::shared_ptr<services::AuthService> authService_;


public:
    /**
     * @brief Constructeur : injecte le service AuthService
     */
    AuthController();

    METHOD_LIST_BEGIN
    // Routes REST propres et standardisées pour l'authentification
    ADD_METHOD_TO(AuthController::registerUser, "/api/auth/register", Post, Options);
    ADD_METHOD_TO(AuthController::login, "/api/auth/login", Post, Options);
    ADD_METHOD_TO(AuthController::me, "/api/auth/me", Get, Options, "middlewares::JwtMiddleware");
    METHOD_LIST_END

    // ==================== HANDLERS DE ROUTES ====================

    /**
     * @brief Handler pour l'inscription utilisateur (POST /api/auth/register)
     * @param req Requête HTTP reçue
     * @param callback Fonction de rappel pour retourner la réponse
     *
     * TODO: Ajouter validation avancée des champs (email, password)
     * TODO: Logger les tentatives d'inscription échouées
     */
    void registerUser(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback);

    /**
     * @brief Handler pour la connexion utilisateur (POST /api/auth/login)
     * @param req Requête HTTP reçue
     * @param callback Fonction de rappel pour retourner la réponse
     *
     * TODO: Ajouter limitation brute-force (rate limit)
     * TODO: Logger les tentatives de connexion échouées
     */
    void login(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback);

    /**
     * @brief Handler pour récupérer le profil utilisateur courant (GET /api/auth/me)
     * @param req Requête HTTP reçue
     * @param callback Fonction de rappel pour retourner la réponse
     *
     * TODO: Vérifier les permissions et la validité du token JWT
     */
    void me(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback);

    // ==================== MÉTHODES UTILITAIRES ====================

    /** @brief Accès au service AuthService (pour tests ou extensions) */
    std::shared_ptr<services::AuthService> getAuthService();
    /** @brief Crée une réponse d'erreur formatée */
    HttpResponsePtr createErrorResponse(const std::string& message, HttpStatusCode status = HttpStatusCode::k400BadRequest);
    /** @brief Crée une réponse d'erreur de validation */
    HttpResponsePtr createValidationErrorResponse(const std::vector<std::string>& errors);
};

} // namespace controllers