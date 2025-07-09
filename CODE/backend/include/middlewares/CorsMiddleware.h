




/************************************************************
 * @file CorsMiddleware.h
 * @brief Middleware CORS global pour gérer les requêtes cross-origin
 *
 * Rôle :
 *   - Ajouter les headers CORS à toutes les réponses HTTP
 *   - Gérer les requêtes OPTIONS (preflight) automatiquement
 *   - Permettre aux applications web de consommer l'API en cross-domain
 *
 * Place dans l'architecture :
 *   - Middleware global, déclaré dans config.json
 *   - S'applique à tous les endpoints de l'API
 *
 * Dépendances :
 *   - Drogon (HttpMiddleware)
 *   - utils/Logger (logs)
 *
 * TODO :
 *   - Restreindre les origins autorisés en production
 *   - Ajouter des tests unitaires sur les cas CORS
 *   - Logger les origines et méthodes pour audit
 ************************************************************/

#pragma once
#include <drogon/HttpMiddleware.h>
#include <drogon/HttpResponse.h>
#include <drogon/HttpRequest.h>

namespace middlewares {

/**
 * @class CorsMiddleware
 * @brief Middleware global pour la gestion automatique du CORS
 *
 * Ce middleware traite TOUTES les requêtes pour :
 * - Ajouter les headers CORS appropriés aux réponses
 * - Répondre automatiquement aux requêtes OPTIONS (preflight)
 * - Permettre aux applications web de consommer l'API
 *
 * PATTERN MIDDLEWARE GLOBAL :
 * - Déclaré dans config.json (pas dans PATH_LIST des contrôleurs)
 * - S'exécute pour TOUS les endpoints automatiquement
 * - Traitement transparent et non-intrusif
 *
 * GESTION PREFLIGHT :
 * - Requêtes OPTIONS → réponse 200 automatique avec headers CORS
 * - Autres requêtes → ajout headers + continuation normale
 */
class CorsMiddleware : public drogon::HttpMiddleware<CorsMiddleware> {
public:
    CorsMiddleware();

    /**
     * @brief Traitement CORS pour toutes les requêtes
     * 
     * LOGIQUE DE TRAITEMENT :
     * 
     * 1. **REQUÊTES OPTIONS (preflight)**
     *    - Génération réponse 200 OK avec headers CORS
     *    - Retour immédiat (pas// ====== COMPLÉMENT ./include/middlewares/CorsMiddleware.h ======

     * @brief Traitement CORS pour toutes les requêtes
     * 
     * LOGIQUE DE TRAITEMENT :
     * 
     * 1. **REQUÊTES OPTIONS (preflight)**
     *    - Génération réponse 200 OK avec headers CORS
     *    - Retour immédiat (pas d'appel au contrôleur)
     *    - Permet au navigateur de valider la requête cross-origin
     * 
     * 2. **AUTRES REQUÊTES (GET, POST, etc.)**
     *    - Ajout des headers CORS à la réponse
     *    - Continuation normale vers le contrôleur
     *    - Headers ajoutés de manière transparente
     * 
     * @param req Requête HTTP entrante
     * @param nextCb Callback pour continuer vers le contrôleur
     * @param mcb Callback pour réponse immédiate (cas OPTIONS)
     * 
     * HEADERS CORS AJOUTÉS :
     * ```
     * Access-Control-Allow-Origin: *
     * Access-Control-Allow-Methods: GET, POST, PUT, DELETE, OPTIONS
     * Access-Control-Allow-Headers: Content-Type, Authorization, X-Requested-With
     * Access-Control-Max-Age: 86400
     * ```
     * 
     * EXEMPLE DE REQUÊTE PREFLIGHT :
     * ```
     * OPTIONS /api/auth/login HTTP/1.1
     * Origin: http://localhost:3000
     * Access-Control-Request-Method: POST
     * Access-Control-Request-Headers: Content-Type, Authorization
     * ```
     * 
     * RÉPONSE PREFLIGHT :
     * ```
     * HTTP/1.1 200 OK
     * Access-Control-Allow-Origin: *
     * Access-Control-Allow-Methods: GET, POST, PUT, DELETE, OPTIONS
     * Access-Control-Allow-Headers: Content-Type, Authorization, X-Requested-With
     * Access-Control-Max-Age: 86400
     * ```
     * 
     * @note Exécuté pour TOUTES les requêtes (middleware global)
     * @note Permet aux webapps de consommer l'API depuis n'importe quel domaine
     * @note En production, remplacer "*" par les domaines autorisés spécifiques
     */
    void invoke(const drogon::HttpRequestPtr& req,
                drogon::MiddlewareNextCallback&& nextCb,
                drogon::MiddlewareCallback&& mcb) override;
};

} // namespace middlewares

