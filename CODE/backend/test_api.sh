#!/bin/bash
# scripts/run_tests.sh

echo "🧪 MetaLeague Manager - Automated Testing"
echo "========================================"

# Vérifier que le serveur backend tourne
if ! curl -s http://localhost:8080/test > /dev/null; then
    echo "❌ Backend server not running on localhost:8080"
    echo "Start the backend first with: ./metalеague_backend"
    exit 1
fi

echo "✅ Backend server is running"

# Compiler les tests si nécessaire
if [ ! -f "./build/test_runner" ]; then
    echo "🔨 Compiling tests..."
    cd build
    cmake .. -DBUILD_TESTS=ON
    make test_runner
    cd ..
fi

echo "🏃 Running test suite..."

# Lancer tous les tests
./build/test_runner all

# Capturer le code de retour
TEST_RESULT=$?

echo ""
if [ $TEST_RESULT -eq 0 ]; then
    echo "✅ All tests passed!"
else
    echo "❌ Some tests failed!"
    echo "Check the output above for details."
fi

echo "📊 Test report saved in test_results_*.json"
echo ""

exit $TEST_RESULT