// ============================================================================
// include/middlewares/JwtMiddleware.h - JWT Authentication Middleware
// ============================================================================

#pragma once
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <functional>
#include <memory>
#include "services/AuthService.h"
#include "utils/JwtUtils.h"
#include "utils/JsonUtils.h"
#include "utils/Logger.h"

namespace middlewares {

namespace beast = boost::beast;
namespace http = beast::http;

template<typename RequestHandler>
class JwtMiddleware {
private:
    std::shared_ptr<services::AuthService> authService_;
    RequestHandler next_;

public:
    JwtMiddleware(std::shared_ptr<services::AuthService> authService, RequestHandler next)
        : authService_(std::move(authService)), next_(std::move(next)) {}

    template<typename Body, typename Allocator>
    void operator()(http::request<Body, http::basic_fields<Allocator>>&& req, 
                   std::function<void(http::response<http::string_body>)> send) {
        
        // Extract Authorization header
        auto authHeader = req[http::field::authorization];
        if (authHeader.empty()) {
            utils::Logger::warn("Missing Authorization header for protected endpoint: " + std::string(req.target()));
            sendUnauthorized(send, "Missing Authorization header");
            return;
        }

        // Extract JWT token
        std::string token = utils::JwtUtils::extractTokenFromHeader(std::string(authHeader));
        if (token.empty()) {
            utils::Logger::warn("Invalid Authorization header format: " + std::string(authHeader));
            sendUnauthorized(send, "Invalid Authorization header format");
            return;
        }

        // Verify token asynchronously
        auto userFuture = authService_->verifyTokenAndGetUserAsync(token);
        auto userOpt = userFuture.get();
        
        if (!userOpt.has_value()) {
            utils::Logger::warn("Invalid or expired JWT token");
            sendUnauthorized(send, "Invalid or expired token");
            return;
        }

        // Add user information to request headers for next handler
        req.set("X-User-ID", std::to_string(userOpt.value().id));
        req.set("X-User-Username", userOpt.value().username);
        req.set("X-User-Email", userOpt.value().email);

        utils::Logger::debug("JWT authentication successful for user: " + userOpt.value().username);

        // Call next handler
        next_(std::move(req));
    }

private:
    // --- CORS: Origin configuration for error responses ---
    // Change this value for production deployment!
    static constexpr const char* DEFAULT_DEV_ORIGIN = "http://localhost:3000";
    template<typename RequestType = void>
    std::string getAllowedOrigin(const RequestType* req = nullptr) {
        // If request is available, try to extract Origin header
        if constexpr (!std::is_same_v<RequestType, void>) {
            if (req) {
                auto origin = (*req)[http::field::origin];
                if (!origin.empty()) {
                    // TODO: In production, validate 'origin' against a whitelist for security
                    return std::string(origin);
                }
            }
        }
        // Fallback: use default dev origin
        return DEFAULT_DEV_ORIGIN;
    }

    // Send a 401 Unauthorized with proper CORS headers
    void sendUnauthorized(std::function<void(http::response<http::string_body>)> send, 
                         const std::string& message) {
        auto errorResponse = utils_json::JsonUtilsHelper::createErrorResponse(message);
        http::response<http::string_body> res{http::status::unauthorized, 11};
        res.set(http::field::content_type, "application/json");
        // --- CORS: set correct origin and credentials ---
        res.set(http::field::access_control_allow_origin, getAllowedOrigin());
        res.set(http::field::access_control_allow_credentials, "true");
        res.body() = errorResponse.dump();
        res.prepare_payload();
        send(std::move(res));
    }
};

// Helper function to create JWT middleware
template<typename RequestHandler>
auto makeJwtMiddleware(std::shared_ptr<services::AuthService> authService, RequestHandler&& handler) {
    return JwtMiddleware<RequestHandler>(std::move(authService), std::forward<RequestHandler>(handler));
}

} // namespace middlewares