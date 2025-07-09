// ============================================================================
// include/utils/DatabaseConnection.h - MySQL Connection Pool
// ============================================================================

#pragma once
#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <cppconn/exception.h>
#include <memory>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <string>
#include "Logger.h"

namespace utils {

class DatabaseConnection {
private:
    std::queue<std::unique_ptr<sql::Connection>> connectionPool_;
    std::mutex poolMutex_;
    std::condition_variable poolCondition_;
    sql::mysql::MySQL_Driver* driver_;
    std::string connectionUrl_;
    std::string username_;
    std::string password_;
    size_t poolSize_;

public:
    DatabaseConnection(const std::string& host, const std::string& database, 
                      const std::string& username, const std::string& password, 
                      size_t poolSize = 10) 
        : connectionUrl_("tcp://" + host + ":3306/" + database)
        , username_(username)
        , password_(password)
        , poolSize_(poolSize) {
        
        driver_ = sql::mysql::get_mysql_driver_instance();
        initializePool();
        Logger::info("Database connection pool initialized with " + std::to_string(poolSize) + " connections");
    }

    ~DatabaseConnection() {
        std::lock_guard<std::mutex> lock(poolMutex_);
        while (!connectionPool_.empty()) {
            connectionPool_.pop();
        }
    }

    std::unique_ptr<sql::Connection> getConnection() {
        std::unique_lock<std::mutex> lock(poolMutex_);
        poolCondition_.wait(lock, [this] { return !connectionPool_.empty(); });
        
        auto connection = std::move(connectionPool_.front());
        connectionPool_.pop();
        
        // Check if connection is still valid
        if (!connection->isValid()) {
            Logger::warn("Invalid connection detected, creating new one");
            connection = createConnection();
        }
        
        return connection;
    }

    void returnConnection(std::unique_ptr<sql::Connection> connection) {
        if (connection && connection->isValid()) {
            std::lock_guard<std::mutex> lock(poolMutex_);
            connectionPool_.push(std::move(connection));
            poolCondition_.notify_one();
        } else {
            Logger::warn("Returned invalid connection, creating replacement");
            std::lock_guard<std::mutex> lock(poolMutex_);
            connectionPool_.push(createConnection());
            poolCondition_.notify_one();
        }
    }

private:
    void initializePool() {
        for (size_t i = 0; i < poolSize_; ++i) {
            connectionPool_.push(createConnection());
        }
    }

    std::unique_ptr<sql::Connection> createConnection() {
        try {
            auto connection = std::unique_ptr<sql::Connection>(
                driver_->connect(connectionUrl_, username_, password_));
            connection->setAutoCommit(true);
            connection->setSchema("metaleague_dev");
            return connection;
        } catch (sql::SQLException& e) {
            Logger::error("Failed to create database connection: " + std::string(e.what()));
            throw;
        }
    }
};

// RAII wrapper for automatic connection return
class ConnectionGuard {
private:
    std::unique_ptr<sql::Connection> connection_;
    DatabaseConnection& pool_;

public:
    ConnectionGuard(DatabaseConnection& pool) : pool_(pool) {
        connection_ = pool_.getConnection();
    }

    ~ConnectionGuard() {
        pool_.returnConnection(std::move(connection_));
    }

    sql::Connection* operator->() {
        return connection_.get();
    }

    sql::Connection& operator*() {
        return *connection_;
    }

    sql::Connection* get() {
        return connection_.get();
    }
};

} // namespace utils