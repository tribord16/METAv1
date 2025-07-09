cmake_minimum_required(VERSION 3.20)
project(MetaLeagueBackend VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_COMPILER_LAUNCHER ccache)
set(CMAKE_CXX_COMPILER_LAUNCHER ccache)

# Find required packages
find_package(Boost REQUIRED COMPONENTS system filesystem)
find_package(OpenSSL REQUIRED)

# Include nlohmann/json
include(FetchContent)
FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz)
FetchContent_MakeAvailable(json)

FetchContent_Declare(jwt-cpp GIT_REPOSITORY https://github.com/Thalhammer/jwt-cpp.git GIT_TAG v0.7.0)
FetchContent_MakeAvailable(jwt-cpp)

# MySQL Connector C++ (manually specify paths if pkg-config doesn't work)
# You can adjust the following paths if the default locations don't work on your system

set(MYSQL_INCLUDE_DIRS /usr/include/cppconn)
set(MYSQL_LIBRARY_DIRS /usr/lib/x86_64-linux-gnu)
set(MYSQL_LIBRARIES mysqlcppconn)

# Add executable
add_executable(${PROJECT_NAME} src/main.cpp)

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${MYSQL_INCLUDE_DIRS}
)

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
    ${Boost_LIBRARIES}
    OpenSSL::SSL
    OpenSSL::Crypto
    nlohmann_json::nlohmann_json
    jwt-cpp::jwt-cpp
    ${MYSQL_LIBRARIES}
)

# Compiler flags
# You might not need this if the MySQL Connector is linked correctly via CMake.
# target_compile_options(${PROJECT_NAME} PRIVATE ${MYSQLCPPCONN_CFLAGS_OTHER})
