
/************************************************************
 * @file CorporationController.h
 * @brief Contrôleur API REST pour la gestion des corporations (Drogon HttpController)
 *
 * Rôle :
 *   - Gérer toutes les routes REST liées aux corporations (CRUD, dashboard)
 *   - Orchestrer la logique métier via CorporationService
 *   - Protéger les routes avec les middlewares JWT et RateLimit
 *
 * Place dans l'architecture :
 *   - Contrôleur HTTP principal pour le domaine "corporation"
 *   - Appelé automatiquement par Drogon via la macro PATH_LIST
 *
 * Dépendances :
 *   - Drogon (HttpController)
 *   - services/CorporationService (logique métier)
 *   - middlewares/JwtMiddleware, RateLimitMiddleware (sécurité)
 *   - dto/corporation (contrats API)
 *
 * TODO :
 *   - Ajouter des validations avancées sur les entrées (nom, unicité, etc.)
 *   - Ajouter des tests unitaires sur chaque handler
 *   - Logger les accès et modifications critiques
 *   - Factoriser la gestion des erreurs et des permissions
 ************************************************************/

#include <drogon/HttpController.h>
#include "services/CorporationService.h"
#include <memory>

using namespace drogon;

namespace controllers {


class CorporationController : public drogon::HttpController<CorporationController> {
private:
    // Service métier pour la gestion des corporations (injection de dépendance)
    std::shared_ptr<services::CorporationService> corpService_;

public:
    /**
     * @brief Constructeur : injecte le service CorporationService
     */
    CorporationController();

    METHOD_LIST_BEGIN
    // Routes REST propres et standardisées pour les corporations
    ADD_METHOD_TO(CorporationController::list, "/api/corporations", Get, "middlewares::JwtMiddleware", "middlewares::RateLimitMiddleware");
    ADD_METHOD_TO(CorporationController::create, "/api/corporations", Post, "middlewares::JwtMiddleware", "middlewares::RateLimitMiddleware");
    ADD_METHOD_TO(CorporationController::get, "/api/corporations/{1}", Get, "middlewares::JwtMiddleware", "middlewares::RateLimitMiddleware");
    ADD_METHOD_TO(CorporationController::update, "/api/corporations/{1}", Put, "middlewares::JwtMiddleware", "middlewares::RateLimitMiddleware");
    ADD_METHOD_TO(CorporationController::remove, "/api/corporations/{1}", Delete, "middlewares::JwtMiddleware", "middlewares::RateLimitMiddleware");
    ADD_METHOD_TO(CorporationController::dashboard, "/api/corporations/{1}/dashboard", Get, "middlewares::JwtMiddleware", "middlewares::RateLimitMiddleware");
    METHOD_LIST_END

    // ==================== HANDLERS DE ROUTES ====================

    /**
     * @brief Handler pour créer une corporation (POST /api/corporations)
     * @param req Requête HTTP reçue
     * @param callback Fonction de rappel pour retourner la réponse
     *
     * TODO: Ajouter validation avancée (unicité nom, format, etc.)
     * TODO: Logger la création de corporation
     */
    void create(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback);

    /**
     * @brief Handler pour lister les corporations de l'utilisateur (GET /api/corporations)
     * @param req Requête HTTP reçue
     * @param callback Fonction de rappel pour retourner la réponse
     */
    void list(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback);

    /**
     * @brief Handler pour obtenir le détail d'une corporation (GET /api/corporations/{id})
     * @param req Requête HTTP reçue
     * @param callback Fonction de rappel pour retourner la réponse
     * @param corpId Identifiant de la corporation
     *
     * TODO: Vérifier que l'utilisateur a bien accès à cette corporation
     */
    void get(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback, int corpId);

    /**
     * @brief Handler pour modifier une corporation (PUT /api/corporations/{id})
     * @param req Requête HTTP reçue
     * @param callback Fonction de rappel pour retourner la réponse
     * @param corpId Identifiant de la corporation
     *
     * TODO: Vérifier les permissions et la validité des données
     */
    void update(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback, int corpId);

    /**
     * @brief Handler pour supprimer une corporation (DELETE /api/corporations/{id})
     * @param req Requête HTTP reçue
     * @param callback Fonction de rappel pour retourner la réponse
     * @param corpId Identifiant de la corporation
     *
     * TODO: Vérifier les permissions et logger la suppression
     */
    void remove(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback, int corpId);

    /**
     * @brief Handler pour obtenir le dashboard d'une corporation (GET /api/corporations/{id}/dashboard)
     * @param req Requête HTTP reçue
     * @param callback Fonction de rappel pour retourner la réponse
     * @param corpId Identifiant de la corporation
     */
    void dashboard(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback, int corpId);

    // ==================== HELPERS ====================

    /** @brief Accès au service CorporationService (pour tests ou extensions) */
    std::shared_ptr<services::CorporationService> getCorporationService();
    /** @brief Crée une réponse d'erreur formatée */
    HttpResponsePtr createErrorResponse(const std::string& message, HttpStatusCode status = HttpStatusCode::k400BadRequest);
    /** @brief Extrait l'ID utilisateur du token JWT */
    int getUserIdFromToken(const HttpRequestPtr& req);
    /** @brief Handler pour les requêtes OPTIONS (CORS) */
    /**
     * @brief Handler pour les requêtes OPTIONS (CORS) sur toutes les routes de ce contrôleur
     *
     * Ce handler répond aux requêtes OPTIONS (préflight CORS) pour toutes les routes REST de ce contrôleur.
     * Il ajoute systématiquement les bons headers CORS pour permettre l'accès cross-origin depuis le frontend.
     *
     * À utiliser dans chaque contrôleur pour garantir que toutes les routes REST sont compatibles CORS.
     *
     * @param req Requête HTTP OPTIONS
     * @param callback Callback pour retourner la réponse
     */
    void handleOptions(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback);
};

} // namespace controllers