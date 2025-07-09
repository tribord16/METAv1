# MetaLeague Backend: Architecture & Implementation Analysis

## 1. Overview
The MetaLeague backend is a modular, service-oriented application built in C++ using the Drogon web framework. It provides RESTful APIs for user authentication, corporation management, and game logic, with a strong focus on security, validation, and scalability. The backend is organized into clear layers: controllers, DTOs, services, repositories, models, middlewares, utils, and tests.

---

## 2. Architecture

### 2.1. Layered Structure
- **Controllers**: Entry points for HTTP requests. Parse input, invoke services, and return API responses.
- **DTOs (Data Transfer Objects)**: Define the structure of data exchanged between client and server. Include validation logic.
- **Services**: Contain business logic. Orchestrate repositories, validation, and utility functions.
- **Repositories**: Abstract database access. Use Drogon ORM for async CRUD and queries.
- **Models**: Represent database entities. Auto-generated from schema (do not edit manually).
- **Middlewares**: Cross-cutting concerns (JWT, CORS, rate limiting).
- **Utils**: Utility classes for cryptography, logging, security, etc.
- **Tests**: Automated API and unit tests.

### 2.2. Main Technologies
- **Drogon**: C++ web framework (async, high performance)
- **PostgreSQL**: Main database
- **jwt-cpp**: JWT token management
- **BCrypt/Linux crypt()**: Password hashing
- **JsonCpp**: JSON parsing/serialization
- **curl**: HTTP client for tests

---

## 3. Key Components & Responsibilities

### 3.1. Authentication & User Management
- **AuthController / AuthService**: Registration, login, JWT issuance, password hashing, validation.
- **UserRepository**: All DB access for users. Async, secure, with error handling.
- **DTOs**: RegisterRequest, LoginRequest, AuthResponse, UserResponse.
- **Security**: Passwords hashed with BCrypt, JWT tokens for stateless auth, input validation.

### 3.2. Corporation Management
- **CorporationController / CorporationService**: CRUD for corporations, business rules (budget, reputation), dashboard data.
- **CorporationRepository**: Async DB access for corporations, queries by user, etc.
- **DTOs**: CreateCorporationRequest, CorporationResponse, CorporationDashboard.

### 3.3. Player Management (in progress)
- **PlayerRepository / PlayerService**: Pool of players, recruitment, stats progression, scouting (partially implemented, ORM migration ongoing).
- **DTOs**: ScoutingRequest, TeamComposition.

### 3.4. Middlewares
- **JwtMiddleware**: Verifies JWT tokens, injects user context.
- **CorsMiddleware**: Handles CORS headers.
- **RateLimitMiddleware/Filter**: Protects against abuse.

### 3.5. Utilities
- **CryptoUtils**: Password hashing/verification.
- **Logger**: Thread-safe, multi-level logging.
- **SecurityUtils**: Input validation, XSS/SQLi protection, secure token generation.

### 3.6. Testing
- **ApiTestFramework**: Automated endpoint tests, JSON validation, performance/security checks.

---

## 4. Implemented Features
- User registration/login with validation and JWT auth
- Corporation CRUD and dashboard
- Secure password storage (BCrypt)
- Async DB access (Drogon ORM)
- DTO-based API contract
- Logging and error handling
- Basic rate limiting and CORS
- Automated API tests

---

## 5. Remaining Work & TODOs
- **Validation**: Strengthen input validation everywhere (see TODOs in code)
- **Logging**: Add more granular logs for DB/service operations
- **Unit Tests**: Expand coverage, especially for edge cases
- **Player Management**: Complete ORM migration, enable all features
- **Security**: Harden error handling, add more sanitization
- **Documentation**: Keep all new code documented and commented
- **Pagination**: Add to all list endpoints
- **Error Codes**: Standardize API error responses

---

## 6. File/Module Map
- See `SUMMARY.md` for a full list of files and their roles.
- Key entry points: `main.cpp`, `controllers/`, `services/`, `repositories/`, `models/`, `middlewares/`, `utils/`, `tests/`

---

## 7. Design Principles
- **Separation of concerns**: Each layer has a clear responsibility
- **Security by design**: Hashing, validation, prepared statements, logging
- **Async everywhere**: Non-blocking DB and HTTP
- **Testability**: DTOs, repositories, and services are mockable
- **Documentation**: All code is commented and documented

---

## 8. Next Steps
- Address all TODOs in code
- Finalize ORM migration for players
- Expand test coverage
- Monitor and improve performance
- Maintain documentation discipline

---

*This document provides a comprehensive analysis of the MetaLeague backend as of July 2025. For details on individual files and changes, see `SUMMARY.md`.*
