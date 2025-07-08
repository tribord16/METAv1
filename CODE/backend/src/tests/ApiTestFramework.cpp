// src/tests/ApiTestFramework.cpp
#include "tests/ApiTestFramework.h"
#include "utils/Logger.h"
#include <curl/curl.h>
#include <iostream>
#include <fstream>
#include <iomanip>

namespace tests {

// Structure pour recevoir les données de curl
struct HttpResponse {
    std::string data;
    long status_code;
    
    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
        ((std::string*)userp)->append((char*)contents, size * nmemb);
        return size * nmemb;
    }
};

ApiTestFramework::ApiTestFramework(const std::string& base_url) : base_url_(base_url) {
    curl_global_init(CURL_GLOBAL_DEFAULT);
    Logger::info("[Tests] API Test Framework initialized with base URL: " + base_url);
}

void ApiTestFramework::setAuthToken(const std::string& token) {
    auth_token_ = token;
    Logger::debug("[Tests] Auth token set for future requests");
}

TestResult ApiTestFramework::post(const std::string& endpoint, const Json::Value& body, const std::string& test_name) {
    TestResult result;
    result.test_name = test_name.empty() ? ("POST " + endpoint) : test_name;
    
    auto start_time = std::chrono::high_resolution_clock::now();
    
    CURL* curl = curl_easy_init();
    if (!curl) {
        result.success = false;
        result.error_message = "Failed to initialize curl";
        return result;
    }
    
    HttpResponse response;
    std::string url = base_url_ + endpoint;
    
    // Sérialiser le JSON
    Json::StreamWriterBuilder builder;
    std::string json_string = Json::writeString(builder, body);
    
    // Headers
    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    if (!auth_token_.empty()) {
        std::string auth_header = "Authorization: Bearer " + auth_token_;
        headers = curl_slist_append(headers, auth_header.c_str());
    }
    
    // Configuration curl
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_string.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, HttpResponse::WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response.data);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L); // 10 secondes timeout
    
    // Exécuter la requête
    CURLcode res = curl_easy_perform(curl);
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response.status_code);
    
    auto end_time = std::chrono::high_resolution_clock::now();
    result.response_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    
    if (res != CURLE_OK) {
        result.success = false;
        result.error_message = curl_easy_strerror(res);
    } else {
        result.success = true;
        result.http_status = response.status_code;
        
        // Parser la réponse JSON
        Json::CharReaderBuilder reader_builder;
        std::string errors;
        std::istringstream response_stream(response.data);
        if (!Json::parseFromStream(reader_builder, response_stream, &result.response_body, &errors)) {
            result.response_body["raw_response"] = response.data;
            result.response_body["parse_error"] = errors;
        }
    }
    
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    
    results_.push_back(result);
    return result;
}

TestResult ApiTestFramework::get(const std::string& endpoint, const std::string& test_name) {
    TestResult result;
    result.test_name = test_name.empty() ? ("GET " + endpoint) : test_name;
    
    auto start_time = std::chrono::high_resolution_clock::now();
    
    CURL* curl = curl_easy_init();
    if (!curl) {
        result.success = false;
        result.error_message = "Failed to initialize curl";
        return result;
    }
    
    HttpResponse response;
    std::string url = base_url_ + endpoint;
    
    // Headers
    struct curl_slist* headers = nullptr;
    if (!auth_token_.empty()) {
        std::string auth_header = "Authorization: Bearer " + auth_token_;
        headers = curl_slist_append(headers, auth_header.c_str());
    }
    
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, HttpResponse::WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response.data);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);
    
    CURLcode res = curl_easy_perform(curl);
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response.status_code);
    
    auto end_time = std::chrono::high_resolution_clock::now();
    result.response_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    
    if (res != CURLE_OK) {
        result.success = false;
        result.error_message = curl_easy_strerror(res);
    } else {
        result.success = true;
        result.http_status = response.status_code;
        
        Json::CharReaderBuilder reader_builder;
        std::string errors;
        std::istringstream response_stream(response.data);
        if (!Json::parseFromStream(reader_builder, response_stream, &result.response_body, &errors)) {
            result.response_body["raw_response"] = response.data;
        }
    }
    
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    
    results_.push_back(result);
    return result;
}

TestResult ApiTestFramework::testRegistration(const std::string& username, const std::string& email, const std::string& password) {
    Json::Value body;
    body["username"] = username;
    body["email"] = email;
    body["password"] = password;
    
    return post("/api/auth/register", body, "User Registration Test");
}

TestResult ApiTestFramework::testLogin(const std::string& username, const std::string& password) {
    Json::Value body;
    body["username"] = username;
    body["password"] = password;
    
    auto result = post("/api/auth/login", body, "User Login Test");
    
    // Extraire le token si login réussi
    if (result.success && result.http_status == 200 && result.response_body.isMember("token")) {
        setAuthToken(result.response_body["token"].asString());
        Logger::debug("[Tests] Auto-extracted auth token from login");
    }
    
    return result;
}

TestResult ApiTestFramework::testProtectedEndpoint(const std::string& endpoint) {
    return get(endpoint, "Protected Endpoint Test: " + endpoint);
}

TestResult ApiTestFramework::testRateLimit(const std::string& endpoint, int requests_count) {
    TestResult result;
    result.test_name = "Rate Limit Test: " + endpoint;

    int blocked_count = 0;
    auto start_time = std::chrono::high_resolution_clock::now();

    Json::Value body;
    body["username"] = "testuser_duplicate";
    body["password"] = "password123";

    for (int i = 0; i < requests_count; ++i) {
        auto test_result = post(endpoint, body, "Rate Limit Request " + std::to_string(i + 1));
        if (test_result.http_status == 429) {
            blocked_count++;
        }
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    result.response_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

    result.success = blocked_count > 0;
    result.response_body["total_requests"] = requests_count;
    result.response_body["blocked_requests"] = blocked_count;

    if (!result.success) {
        result.error_message = "Rate limiting not working - no requests blocked";
    }

    results_.push_back(result);
    return result;
}

bool ApiTestFramework::assertStatus(const TestResult& result, int expected_status) {
    bool passed = result.http_status == expected_status;
    if (!passed) {
        Logger::warn("[Tests] Status assertion failed for " + result.test_name + 
                    ": expected " + std::to_string(expected_status) + 
                    ", got " + std::to_string(result.http_status));
    }
    return passed;
}

bool ApiTestFramework::assertJsonField(const TestResult& result, const std::string& field, const Json::Value& expected) {
    if (!result.response_body.isMember(field)) {
        Logger::warn("[Tests] Field '" + field + "' not found in response for " + result.test_name);
        return false;
    }
    
    bool passed = result.response_body[field] == expected;
    if (!passed) {
        Logger::warn("[Tests] Field assertion failed for " + result.test_name + 
                    " field '" + field + "'");
    }
    return passed;
}

bool ApiTestFramework::assertResponseTime(const TestResult& result, int max_ms) {
    bool passed = result.response_time.count() <= max_ms;
    if (!passed) {
        Logger::warn("[Tests] Response time assertion failed for " + result.test_name + 
                    ": " + std::to_string(result.response_time.count()) + "ms > " + std::to_string(max_ms) + "ms");
    }
    return passed;
}

void ApiTestFramework::runAuthTests() {
    Logger::info("[Tests] Running complete authentication test suite...");
    
    // Test 1: Registration avec données valides
    auto reg_result = testRegistration("testuser_" + std::to_string(time(nullptr)), "test@example.com", "password123");
    assertStatus(reg_result, 201);
    assertJsonField(reg_result, "success", Json::Value(true));
    assertResponseTime(reg_result, 500);
    
    // Test 2: Registration avec username déjà pris
    auto reg_duplicate = testRegistration("testuser_duplicate", "test2@example.com", "password123");
    // Premier appel devrait réussir
    auto reg_duplicate2 = testRegistration("testuser_duplicate", "test3@example.com", "password123");
    assertStatus(reg_duplicate2, 400); // Deuxième appel devrait échouer
    
    // Test 3: Login avec credentials valides
    auto login_result = testLogin("testuser_duplicate", "password123");
    assertStatus(login_result, 200);
    assertJsonField(login_result, "success", Json::Value(true));
    
    // Test 4: Login avec credentials invalides
    auto login_invalid = testLogin("testuser_duplicate", "wrongpassword");
    assertStatus(login_invalid, 401);
    assertJsonField(login_invalid, "success", Json::Value(false));
    
    // Test 5: Accès endpoint protégé avec token valide
    auto protected_result = testProtectedEndpoint("/api/auth/me");
    assertStatus(protected_result, 200);
    
    // Test 6: Accès endpoint protégé sans token
    setAuthToken(""); // Clear token
    auto unprotected_result = testProtectedEndpoint("/api/auth/me");
    assertStatus(unprotected_result, 401);
    
    Logger::info("[Tests] Authentication test suite completed");
}

void ApiTestFramework::runSecurityTests() {
    Logger::info("[Tests] Running security test suite...");
    
    // Test 1: Injection SQL dans username
    auto sql_injection = testRegistration("'; DROP TABLE users; --", "hack@example.com", "password123");
    assertStatus(sql_injection, 400); // Devrait être rejeté
    
    // Test 2: XSS dans username
    auto xss_test = testRegistration("<script>alert('xss')</script>", "xss@example.com", "password123");
    assertStatus(xss_test, 400); // Devrait être rejeté
    
    // Test 3: Username trop long
    std::string long_username(1000, 'a');
    auto long_user_test = testRegistration(long_username, "long@example.com", "password123");
    assertStatus(long_user_test, 400);
    
    // Test 4: Email invalide
    auto invalid_email = testRegistration("validuser", "notanemail", "password123");
    assertStatus(invalid_email, 400);
    
    // Test 5: Password trop court
    auto short_password = testRegistration("validuser2", "valid@example.com", "123");
    assertStatus(short_password, 400);
    
    // Test 6: Rate limiting sur login
    auto rate_limit_result = testRateLimit("/api/auth/login", 10);
    // Au moins quelques requêtes devraient être bloquées
    
    Logger::info("[Tests] Security test suite completed");
}

void ApiTestFramework::runPerformanceTests() {
    Logger::info("[Tests] Running performance test suite...");
    
    // Test de charge basique
    auto start_time = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < 50; ++i) {
        auto result = get("/test", "Performance Test " + std::to_string(i));
        assertResponseTime(result, 200); // Max 200ms par requête
    }
    
    auto end_time = std::chrono::high_resolution_clock::now();
    auto total_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    
    Logger::info("[Tests] 50 requests completed in " + std::to_string(total_time.count()) + "ms");
    Logger::info("[Tests] Average response time: " + std::to_string(total_time.count() / 50) + "ms");
    
    Logger::info("[Tests] Performance test suite completed");
}

void ApiTestFramework::printResults() {
    std::cout << "\n" << std::string(80, '=') << "\n";
    std::cout << "API TEST RESULTS\n";
    std::cout << std::string(80, '=') << "\n";
    
    int passed = 0, failed = 0;
    
    for (const auto& result : results_) {
        std::string status = result.success ? "✅ PASS" : "❌ FAIL";
        std::cout << std::left << std::setw(60) << result.test_name 
                  << " | " << status 
                  << " | " << std::setw(4) << result.http_status 
                  << " | " << std::setw(6) << result.response_time.count() << "ms\n";
        
        if (result.success) passed++; else failed++;
        
        if (!result.success && !result.error_message.empty()) {
            std::cout << "    Error: " << result.error_message << "\n";
        }
    }
    
    std::cout << std::string(80, '-') << "\n";
    std::cout << "SUMMARY: " << passed << " passed, " << failed << " failed\n";
    std::cout << std::string(80, '=') << "\n\n";
}

bool ApiTestFramework::allTestsPassed() {
    for (const auto& result : results_) {
        if (!result.success) return false;
    }
    return true;
}

void ApiTestFramework::saveResultsToFile(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        Logger::error("[Tests] Failed to open file for writing: " + filename);
        return;
    }
    
    Json::Value report;
    report["timestamp"] = std::time(nullptr);
    report["total_tests"] = static_cast<int>(results_.size());
    
    int passed = 0;
    Json::Value tests(Json::arrayValue);
    
    for (const auto& result : results_) {
        Json::Value test_json;
        test_json["name"] = result.test_name;
        test_json["success"] = result.success;
        test_json["http_status"] = result.http_status;
        test_json["response_time_ms"] = static_cast<int>(result.response_time.count());
        test_json["error_message"] = result.error_message;
        
        tests.append(test_json);
        if (result.success) passed++;
    }
    
    report["tests"] = tests;
    report["passed"] = passed;
    report["failed"] = static_cast<int>(results_.size()) - passed;
    
    Json::StreamWriterBuilder builder;
    builder["indentation"] = "  ";
    file << Json::writeString(builder, report);
    
    Logger::info("[Tests] Test results saved to: " + filename);
}

} // namespace tests