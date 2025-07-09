// ============================================================================
// include/middlewares/CorsMiddleware.h - CORS Middleware
// ============================================================================

#pragma once
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <functional>
#include "utils/Logger.h"

namespace middlewares {

namespace beast = boost::beast;
namespace http = beast::http;

template<typename RequestHandler>
class CorsMiddleware {
private:
    RequestHandler next_;

public:
    explicit CorsMiddleware(RequestHandler next) : next_(std::move(next)) {}

    template<typename Body, typename Allocator>
    void operator()(http::request<Body, http::basic_fields<Allocator>>&& req, 
                   std::function<void(http::response<http::string_body>)> send) {
        
        // Handle preflight OPTIONS requests
        if (req.method() == http::verb::options) {
            utils::Logger::debug("Handling CORS preflight request for: " + std::string(req.target()));
            
            http::response<http::string_body> res{http::status::ok, req.version()};
            addCorsHeaders(res, req);
            res.body() = "";
            res.prepare_payload();
            
            send(std::move(res));
            return;
        }

        // Correction : la plupart des handlers attendent un seul argument (la requête)
        // Si tu veux une réponse CORS, il faut gérer la réponse dans le handler final
        next_(std::move(req), [this, req, send](http::response<http::string_body> response) mutable {
            addCorsHeaders(response, req);
            send(std::move(response));
        });
    }

private:
    template<typename RequestType>
    void addCorsHeaders(http::response<http::string_body>& res, const RequestType& req) {
        // Get Origin header from request
        auto origin = req[http::field::origin];
        if (!origin.empty()) {
            // Set the exact origin (never *)
            res.set(http::field::access_control_allow_origin, origin);
        } else {
            // Fallback: allow localhost:3000 (dev only)
            res.set(http::field::access_control_allow_origin, "http://localhost:3000");
        }
        res.set(http::field::access_control_allow_credentials, "true");
        res.set(http::field::access_control_allow_methods, "GET, POST, PUT, DELETE, OPTIONS");
        res.set(http::field::access_control_allow_headers, "Content-Type, Authorization, X-Requested-With");
        res.set(http::field::access_control_max_age, "86400");
    }
};

// Helper function to create CORS middleware
template<typename RequestHandler>
auto makeCorsMiddleware(RequestHandler&& handler) {
    return CorsMiddleware<RequestHandler>(std::forward<RequestHandler>(handler));
}

} // namespace middlewares