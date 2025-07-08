// include/tests/ApiTestFramework.h
/**
 * @file ApiTestFramework.h
 * @brief Framework de tests API pour automatiser les tests des endpoints
 * 
 * FONCTIONNALITÉS :
 * - Tests automatisés de tous les endpoints auth
 * - Validation des réponses JSON
 * - Tests de performance basiques
 * - Tests de sécurité (rate limiting, validation)
 * - Génération de rapports de tests
 */

#pragma once
#include <string>
#include <functional>
#include <vector>
#include <chrono>
#include <json/json.h>

namespace tests {

struct TestResult {
    std::string test_name;
    bool success;
    std::string error_message;
    int http_status;
    std::chrono::milliseconds response_time;
    Json::Value response_body;
};

class ApiTestFramework {
private:
    std::string base_url_;
    std::string auth_token_;
    std::vector<TestResult> results_;
    
public:
    explicit ApiTestFramework(const std::string& base_url = "http://localhost:8080");
    
    // Configuration
    void setAuthToken(const std::string& token);
    
    // Méthodes HTTP
    TestResult get(const std::string& endpoint, const std::string& test_name = "");
    TestResult post(const std::string& endpoint, const Json::Value& body, const std::string& test_name = "");
    TestResult put(const std::string& endpoint, const Json::Value& body, const std::string& test_name = "");
    TestResult del(const std::string& endpoint, const std::string& test_name = "");
    
    // Tests spécialisés
    TestResult testRegistration(const std::string& username, const std::string& email, const std::string& password);
    TestResult testLogin(const std::string& username, const std::string& password);
    TestResult testProtectedEndpoint(const std::string& endpoint);
    TestResult testRateLimit(const std::string& endpoint, int requests_count);
    
    // Assertions
    bool assertStatus(const TestResult& result, int expected_status);
    bool assertJsonField(const TestResult& result, const std::string& field, const Json::Value& expected);
    bool assertResponseTime(const TestResult& result, int max_ms);
    
    // Rapports
    void printResults();
    bool allTestsPassed();
    void saveResultsToFile(const std::string& filename);
    
    // Suites de tests complètes
    void runAuthTests();
    void runSecurityTests();
    void runPerformanceTests();
};

} // namespace tests