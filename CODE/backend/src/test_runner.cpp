// src/test_runner.cpp
/**
 * @file test_runner.cpp
 * @brief Exécutable standalone pour lancer tous les tests API
 * 
 * UTILISATION :
 * ./test_runner                    # Tests complets
 * ./test_runner auth               # Tests auth seulement
 * ./test_runner security           # Tests sécurité seulement
 * ./test_runner performance        # Tests performance seulement
 */

#include "tests/ApiTestFramework.h"
#include "utils/Logger.h"
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    std::string base_url = "http://localhost:8080";
    std::string test_suite = "all";
    
    // Parser les arguments
    if (argc > 1) {
        test_suite = argv[1];
    }
    if (argc > 2) {
        base_url = argv[2];
    }
    
    std::cout << "🧪 MetaLeague Manager - API Test Runner\n";
    std::cout << "Testing against: " << base_url << "\n";
    std::cout << "Test suite: " << test_suite << "\n\n";
    
    tests::ApiTestFramework tester(base_url);
    
    // Vérifier que le serveur répond
    auto health_check = tester.get("/test", "Health Check");
    if (!health_check.success || health_check.http_status != 200) {
        std::cout << "❌ Server not responding at " << base_url << "\n";
        std::cout << "Make sure the backend is running before running tests.\n";
        return 1;
    }
    
    std::cout << "✅ Server is responding\n\n";
    
    // Lancer les suites de tests
    if (test_suite == "all" || test_suite == "auth") {
        tester.runAuthTests();
    }
    
    if (test_suite == "all" || test_suite == "security") {
        tester.runSecurityTests();
    }
    
    if (test_suite == "all" || test_suite == "performance") {
        tester.runPerformanceTests();
    }
    
    // Afficher les résultats
    tester.printResults();
    
    // Sauvegarder le rapport
    std::string timestamp = std::to_string(std::time(nullptr));
    tester.saveResultsToFile("test_results_" + timestamp + ".json");
    
    // Code de retour
    return tester.allTestsPassed() ? 0 : 1;
}