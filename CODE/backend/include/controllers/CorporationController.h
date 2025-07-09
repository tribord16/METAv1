// ============================================================================
// include/controllers/CorporationController.h - Corporation HTTP Endpoints (Continued)
// ============================================================================

#pragma once
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <nlohmann/json.hpp>
#include <memory>
#include "services/CorporationService.h"
#include "dto/corporation/CreateCorporationRequest.h"
#include "utils/JsonUtils.h"
#include "utils/Logger.h"


namespace controllers {

namespace beast = boost::beast;
namespace http = beast::http;

class CorporationController {
private:
    std::shared_ptr<services::CorporationService> corpService_;

public:
    explicit CorporationController(std::shared_ptr<services::CorporationService> corpService)
        : corpService_(std::move(corpService)) {}

    // POST /api/corporations
    template<typename Body, typename Allocator>
    void handleCreate(http::request<Body, http::basic_fields<Allocator>>&& req,
                     std::function<void(http::response<http::string_body>)> send) {
        
        utils::Logger::info("Processing create corporation request");
        
        try {
            // Extract user ID from JWT middleware
            auto userIdHeader = req["X-User-ID"];
            if (userIdHeader.empty()) {
                sendUnauthorized(send, "User authentication required");
                return;
            }
            
            int userId = std::stoi(std::string(userIdHeader));
            
            // Parse JSON body
            auto json = nlohmann::json::parse(req.body());
            auto createRequest = dto::corporation::CreateCorporationRequest::fromJson(json);
            
            // Process creation asynchronously
            auto responseFuture = corpService_->createCorporationAsync(createRequest, userId);
            auto apiResponse = responseFuture.get();
            
            // Create HTTP response
            http::response<http::string_body> res{
                apiResponse.success ? http::status::created : http::status::bad_request, 
                req.version()
            };
            
            res.set(http::field::content_type, "application/json");
            res.body() = apiResponse.toJson().dump();
            res.prepare_payload();
            
            send(std::move(res));
            
        } catch (const nlohmann::json::parse_error& e) {
            utils::Logger::warn("Invalid JSON in create corporation request: " + std::string(e.what()));
            sendBadRequest(send, "Invalid JSON format");
        } catch (const std::exception& e) {
            utils::Logger::error("Exception in handleCreate: " + std::string(e.what()));
            sendInternalError(send, "Internal server error");
        }
    }

    // GET /api/corporations
    template<typename Body, typename Allocator>
    void handleList(http::request<Body, http::basic_fields<Allocator>>&& req,
                   std::function<void(http::response<http::string_body>)> send) {
        
        utils::Logger::info("Processing list corporations request");
        
        try {
            // Extract user ID from JWT middleware
            auto userIdHeader = req["X-User-ID"];
            if (userIdHeader.empty()) {
                sendUnauthorized(send, "User authentication required");
                return;
            }
            
            int userId = std::stoi(std::string(userIdHeader));
            
            // Get corporations asynchronously
            utils::Logger::info("Fetching corporations for user ID: " + std::to_string(userId));
            auto responseFuture = corpService_->getCorporationsByUserAsync(userId);
            auto apiResponse = responseFuture.get();
            
            // Create HTTP response
            http::response<http::string_body> res{http::status::ok, req.version()};
            res.set(http::field::content_type, "application/json");
            res.body() = apiResponse.toJson().dump();
            res.prepare_payload();
            
            send(std::move(res));
            
        } catch (const std::exception& e) {
            utils::Logger::error("Exception in handleList: " + std::string(e.what()));
            sendInternalError(send, "Internal server error");
        }
    }

    // GET /api/corporations/{id}
    template<typename Body, typename Allocator>
    void handleGet(http::request<Body, http::basic_fields<Allocator>>&& req,
                  std::function<void(http::response<http::string_body>)> send,
                  int corporationId) {
        
        utils::Logger::info("Processing get corporation request for ID: " + std::to_string(corporationId));
        
        try {
            // Extract user ID from JWT middleware
            auto userIdHeader = req["X-User-ID"];
            if (userIdHeader.empty()) {
                sendUnauthorized(send, "User authentication required");
                return;
            }
            
            int userId = std::stoi(std::string(userIdHeader));
            
            // Get corporation asynchronously
            auto responseFuture = corpService_->getCorporationByIdAsync(corporationId, userId);
            auto apiResponse = responseFuture.get();
            
            // Create HTTP response
            http::response<http::string_body> res{
                apiResponse.success ? http::status::ok : http::status::not_found, 
                req.version()
            };
            
            res.set(http::field::content_type, "application/json");
            res.body() = apiResponse.toJson().dump();
            res.prepare_payload();
            
            send(std::move(res));
            
        } catch (const std::exception& e) {
            utils::Logger::error("Exception in handleGet: " + std::string(e.what()));
            sendInternalError(send, "Internal server error");
        }
    }

private:
    void sendBadRequest(std::function<void(http::response<http::string_body>)> send, 
                       const std::string& message) {
        auto errorResponse = utils_json::JsonUtilsHelper::createErrorResponse(message);
        
        http::response<http::string_body> res{http::status::bad_request, 11};
        res.set(http::field::content_type, "application/json");
        res.body() = errorResponse.dump();
        res.prepare_payload();
        
        send(std::move(res));
    }

    void sendUnauthorized(std::function<void(http::response<http::string_body>)> send, 
                         const std::string& message) {
        auto errorResponse = utils_json::JsonUtilsHelper::createErrorResponse(message);
        
        http::response<http::string_body> res{http::status::unauthorized, 11};
        res.set(http::field::content_type, "application/json");
        res.body() = errorResponse.dump();
        res.prepare_payload();
        
        send(std::move(res));
    }

    void sendInternalError(std::function<void(http::response<http::string_body>)> send, 
                          const std::string& message) {
        auto errorResponse = utils_json::JsonUtilsHelper::createErrorResponse(message);
        
        http::response<http::string_body> res{http::status::internal_server_error, 11};
        res.set(http::field::content_type, "application/json");
        res.body() = errorResponse.dump();
        res.prepare_payload();
        
        send(std::move(res));
    }
};

} // namespace controllers