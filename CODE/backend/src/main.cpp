// ============================================================================
// src/main.cpp - Main Server Implementation with Boost.Beast
// ============================================================================

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/strand.hpp>
#include <boost/config.hpp>
#include <algorithm>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <vector>

// Application includes
#include "controllers/AuthController.h"
#include "controllers/CorporationController.h"
#include "services/AuthService.h"
#include "services/CorporationService.h"
#include "repositories/UserRepository.h"
#include "repositories/CorporationRepository.h"
#include "middlewares/JwtMiddleware.h"
#include "middlewares/CorsMiddleware.h"
#include "utils/DatabaseConnection.h"
#include "utils/Logger.h"
#include "utils/JsonUtils.h"

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using tcp = boost::asio::ip::tcp;

// ============================================================================
// HTTP Session - Handles individual HTTP connections
// ============================================================================

class HttpSession : public std::enable_shared_from_this<HttpSession> {
    // TODO: Utiliser beast::tcp_stream pour la gestion du timeout et des opérations asynchrones robustes
    beast::tcp_stream stream_;
    beast::flat_buffer buffer_;
    std::shared_ptr<controllers::AuthController> authController_;
    std::shared_ptr<controllers::CorporationController> corpController_;
    std::shared_ptr<services::AuthService> authService_;

public:
    explicit HttpSession(tcp::socket&& socket, 
                        std::shared_ptr<controllers::AuthController> authController,
                        std::shared_ptr<controllers::CorporationController> corpController,
                        std::shared_ptr<services::AuthService> authService)
        : stream_(std::move(socket)) // Correction : on utilise beast::tcp_stream
        , authController_(std::move(authController))
        , corpController_(std::move(corpController))
        , authService_(std::move(authService)) {}

    void run() {
        // TODO: Vérifier la gestion du strand/executor pour éviter les data races
        net::dispatch(stream_.get_executor(),
                     beast::bind_front_handler(&HttpSession::doRead, shared_from_this()));
    }

private:
    void doRead() {
        // Make the request empty before reading, otherwise the operation behavior is undefined
        req_ = {};

        // TODO: Timeout configurable par requête (ex: via config ou header)
        stream_.expires_after(std::chrono::seconds(30)); // Correction : timeout sur la connexion

        // Read a request
        http::async_read(stream_, buffer_, req_,
                        beast::bind_front_handler(&HttpSession::onRead, shared_from_this()));
    }

    void onRead(beast::error_code ec, std::size_t bytes_transferred) {
        boost::ignore_unused(bytes_transferred);

        if (ec == http::error::end_of_stream) {
            return doClose();
        }

        if (ec) {
            utils::Logger::error("HTTP read error: " + ec.message());
            return;
        }

        // Send the response
        handleRequest(std::move(req_));
    }

    void handleRequest(http::request<http::string_body>&& req) {
        // TODO: Logger tous les headers de la requête pour debug
        auto send = [self = shared_from_this()](http::response<http::string_body>&& res) {
            self->res_ = std::move(res);
            self->writeResponse();
        };

        // Correction : handler CORS compatible signature 2 arguments (req, send)
        // TODO: Permettre la configuration dynamique des CORS (origines, headers, etc)
        auto corsHandler = middlewares::makeCorsMiddleware([this](auto&& req, auto&& send) {
            this->routeRequest(std::move(req), send);
        });

        corsHandler(std::move(req), send);
    }

    void routeRequest(http::request<http::string_body>&& req, 
                     std::function<void(http::response<http::string_body>)> send) {
        
        std::string target = std::string(req.target());
        auto method = req.method();


        utils::Logger::debug("Routing request: " + std::string(req.method_string()) + " " + target);

        // Auth routes (public)
        if (target == "/api/auth/register" && method == http::verb::post) {
            authController_->handleRegister(std::move(req), send);
        }
        else if (target == "/api/auth/login" && method == http::verb::post) {
            authController_->handleLogin(std::move(req), send);
        }
        // Protected auth routes
        else if (target == "/api/auth/me" && method == http::verb::get) {
            auto jwtHandler = middlewares::makeJwtMiddleware(authService_, 
                [this, send](auto&& req) {
                    this->authController_->handleMe(std::move(req), send);
                });
            jwtHandler(std::move(req), send);
        }
        // Corporation routes (all protected)
        else if (target == "/api/corporations" && method == http::verb::post) {
            auto jwtHandler = middlewares::makeJwtMiddleware(authService_, 
                [this, send](auto&& req) {
                    this->corpController_->handleCreate(std::move(req), send);
                });
            jwtHandler(std::move(req), send);
        }
        else if (target == "/api/corporations" && method == http::verb::get) {
            auto jwtHandler = middlewares::makeJwtMiddleware(authService_, 
                [this, send](auto&& req) {
                    this->corpController_->handleList(std::move(req), send);
                });
            jwtHandler(std::move(req), send);
        }
        // Corporation by ID routes
        else if (target.starts_with("/api/corporations/") && target.find("/", 18) == std::string::npos) {
            try {
                // Extract corporation ID from URL
                std::string idStr = target.substr(18); // Remove "/api/corporations/"
                int corporationId = std::stoi(idStr);
                
                if (method == http::verb::get) {
                    auto jwtHandler = middlewares::makeJwtMiddleware(authService_, 
                        [this, send, corporationId](auto&& req) {
                            this->corpController_->handleGet(std::move(req), send, corporationId);
                        });
                    jwtHandler(std::move(req), send);
                } else {
                    sendNotFound(send, "Method not allowed for this endpoint");
                }
            } catch (const std::invalid_argument&) {
                sendBadRequest(send, "Invalid corporation ID");
            }
        }
        // Health check endpoint
        else if (target == "/health" && method == http::verb::get) {
            auto response = utils_json::JsonUtilsHelper::createResponse(true, "Server is healthy");
            
            http::response<http::string_body> res{http::status::ok, req.version()};
            res.set(http::field::content_type, "application/json");
            res.body() = response.dump();
            res.prepare_payload();
            
            send(std::move(res));
        }
        // 404 - Not Found
        else {
            sendNotFound(send, "Endpoint not found");
        }
    }

    void sendNotFound(std::function<void(http::response<http::string_body>)> send, 
                     const std::string& message) {
        auto errorResponse = utils_json::JsonUtilsHelper::createErrorResponse(message);
        
        http::response<http::string_body> res{http::status::not_found, 11};
        res.set(http::field::content_type, "application/json");
        res.body() = errorResponse.dump();
        res.prepare_payload();
        
        send(std::move(res));
    }

    void sendBadRequest(std::function<void(http::response<http::string_body>)> send, 
                       const std::string& message) {
        auto errorResponse = utils_json::JsonUtilsHelper::createErrorResponse(message);
        
        http::response<http::string_body> res{http::status::bad_request, 11};
        res.set(http::field::content_type, "application/json");
        res.body() = errorResponse.dump();
        res.prepare_payload();
        
        send(std::move(res));
    }

    void writeResponse() {
        auto self = shared_from_this();
        res_.set(http::field::content_type, "application/json");

        // TODO: Gérer les erreurs d'écriture réseau (ex: client déconnecté)
        http::async_write(stream_, res_,
                         [self](beast::error_code ec, std::size_t) {
                             self->onWrite(ec, self->res_.need_eof());
                         });
    }

    void onWrite(beast::error_code ec, bool close) {
        if (ec) {
            utils::Logger::error("HTTP write error: " + ec.message());
            return;
        }

        if (close) {
            return doClose();
        }

        // Read another request
        doRead();
    }

    void doClose() {
        beast::error_code ec;
        // TODO: Vérifier la gestion correcte de la fermeture de la connexion
        stream_.socket().shutdown(tcp::socket::shutdown_send, ec);
    }

    http::request<http::string_body> req_;
    http::response<http::string_body> res_;
};

// ============================================================================
// HTTP Listener - Accepts incoming connections
// TODO: Ajouter un mécanisme de backoff/retry si la base de données ou le port est indisponible
// ============================================================================

class Listener : public std::enable_shared_from_this<Listener> {
    net::io_context& ioc_;
    tcp::acceptor acceptor_;
    std::shared_ptr<controllers::AuthController> authController_;
    std::shared_ptr<controllers::CorporationController> corpController_;
    std::shared_ptr<services::AuthService> authService_;

public:
    Listener(net::io_context& ioc, tcp::endpoint endpoint,
             std::shared_ptr<controllers::AuthController> authController,
             std::shared_ptr<controllers::CorporationController> corpController,
             std::shared_ptr<services::AuthService> authService)
        : ioc_(ioc)
        , acceptor_(net::make_strand(ioc))
        , authController_(std::move(authController))
        , corpController_(std::move(corpController))
        , authService_(std::move(authService)) {
        
        beast::error_code ec;

        // Open the acceptor
        acceptor_.open(endpoint.protocol(), ec);
        if (ec) {
            utils::Logger::error("Failed to open acceptor: " + ec.message());
            return;
        }

        // Allow address reuse
        acceptor_.set_option(net::socket_base::reuse_address(true), ec);
        if (ec) {
            utils::Logger::error("Failed to set socket options: " + ec.message());
            return;
        }
        
        

        // Bind to the server address
        acceptor_.bind(endpoint, ec);
        if (ec) {
            utils::Logger::error("Failed to bind acceptor: " + ec.message());
            return;
        }

        // Start listening for connections
        utils::Logger::info("Starting to listen on: " + endpoint.address().to_string() + ":" + std::to_string(endpoint.port()));
        acceptor_.listen(net::socket_base::max_listen_connections, ec);
        if (ec) {
            utils::Logger::error("Failed to listen: " + ec.message());
            return;
        }
    }

    void run() {
        doAccept();
    }

private:
    void doAccept() {
        acceptor_.async_accept(net::make_strand(ioc_),
                              beast::bind_front_handler(&Listener::onAccept, shared_from_this()));
    }

    void onAccept(beast::error_code ec, tcp::socket socket) {
        if (ec) {
            utils::Logger::error("Accept error: " + ec.message());
        } else {
            // TODO: Ajouter une gestion fine des exceptions lors de la création de session
            std::make_shared<HttpSession>(std::move(socket), authController_, corpController_, authService_)->run();
        }

        // Accept another connection
        doAccept();
    }
};

// ============================================================================
// TODO: Ajouter un endpoint /metrics pour la supervision (Prometheus, etc)
// ============================================================================

int main(int argc, char* argv[]) {
    try {
        // Initialize logger
        utils::Logger::setLogLevel(utils::LogLevel::DEBUG);
        utils::Logger::info("Starting MetaLeague Backend Server...");

        // Configuration
        std::string host = "127.0.0.1";
        std::string dbHost = "localhost";
        std::string dbName = "meta_league_dev";
        std::string dbUser = "metaldev";
        std::string dbPassword = "MetaLeague2024!";
        unsigned short port = 8090;
        int threads = std::max<int>(1, std::thread::hardware_concurrency());

        // Parse command line arguments
        if (argc >= 2) {
            host = argv[1];
        }
        if (argc >= 3) {
            port = static_cast<unsigned short>(std::atoi(argv[2]));
        }
        if (argc >= 4) {
            threads = std::atoi(argv[3]);
        }

        utils::Logger::info("Server configuration:");
        utils::Logger::info("  Host: " + host);
        utils::Logger::info("  Port: " + std::to_string(port));
        utils::Logger::info("  Threads: " + std::to_string(threads));

        // Initialize database connection pool
        auto dbPool = std::make_shared<utils::DatabaseConnection>(dbHost, dbName, dbUser, dbPassword, 10);
        utils::Logger::info("Database connection pool initialized");

        // Initialize repositories
        auto userRepo = std::make_shared<repositories::UserRepository>(dbPool);
        auto corpRepo = std::make_shared<repositories::CorporationRepository>(dbPool);
        utils::Logger::info("Repositories initialized");

        // Initialize services
        auto authService = std::make_shared<services::AuthService>(userRepo);
        auto corpService = std::make_shared<services::CorporationService>(corpRepo);
        utils::Logger::info("Services initialized");

        // Initialize controllers
        auto authController = std::make_shared<controllers::AuthController>(authService);
        auto corpController = std::make_shared<controllers::CorporationController>(corpService);
        utils::Logger::info("Controllers initialized");

        // Create I/O context and endpoint
        net::io_context ioc{threads};
        auto const address = net::ip::make_address(host);
        auto const endpoint = tcp::endpoint{address, port};

        // Create and launch the listener
        std::make_shared<Listener>(ioc, endpoint, authController, corpController, authService)->run();

        utils::Logger::info("🚀 MetaLeague Backend Server started successfully!");
        utils::Logger::info("📡 Listening on http://" + host + ":" + std::to_string(port));
        utils::Logger::info("🔗 Available endpoints:");
        utils::Logger::info("  POST /api/auth/register   - User registration");
        utils::Logger::info("  POST /api/auth/login      - User login");
        utils::Logger::info("  GET  /api/auth/me         - Get current user (JWT protected)");
        utils::Logger::info("  POST /api/corporations    - Create corporation (JWT protected)");
        utils::Logger::info("  GET  /api/corporations    - List user corporations (JWT protected)");
        utils::Logger::info("  GET  /api/corporations/{id} - Get corporation details (JWT protected)");
        utils::Logger::info("  GET  /health              - Health check");

        // Run the I/O service on the requested number of threads
        std::vector<std::thread> v;
        v.reserve(threads - 1);
        for (auto i = threads - 1; i > 0; --i) {
            v.emplace_back([&ioc] { ioc.run(); });
        }
        ioc.run();

        // Wait for all threads to complete
        for (auto& t : v) {
            t.join();
        }

    } catch (const std::exception& e) {
        utils::Logger::error("Fatal error: " + std::string(e.what()));
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}