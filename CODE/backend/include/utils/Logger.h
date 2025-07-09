// ============================================================================
// include/utils/Logger.h - High-Performance Logging System
// ============================================================================

#pragma once
#include <iostream>
#include <string>
#include <chrono>
#include <iomanip>
#include <sstream>

namespace utils {

enum class LogLevel {
    DEBUG = 0,
    INFO = 1,
    WARN = 2,
    ERROR = 3
};

class Logger {
private:
    static LogLevel currentLevel_;

    static std::string getCurrentTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()) % 1000;
        
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
        ss << '.' << std::setfill('0') << std::setw(3) << ms.count();
        return ss.str();
    }

    static std::string levelToString(LogLevel level) {
        switch (level) {
            case LogLevel::DEBUG: return "DEBUG";
            case LogLevel::INFO:  return "INFO ";
            case LogLevel::WARN:  return "WARN ";
            case LogLevel::ERROR: return "ERROR";
            default: return "UNKNOWN";
        }
    }

public:
    static void setLogLevel(LogLevel level) {
        currentLevel_ = level;
    }

    static void log(LogLevel level, const std::string& message) {
        if (level >= currentLevel_) {
            std::cout << "[" << getCurrentTimestamp() << "] "
                      << "[" << levelToString(level) << "] "
                      << message << std::endl;
        }
    }

    static void debug(const std::string& message) { log(LogLevel::DEBUG, message); }
    static void info(const std::string& message) { log(LogLevel::INFO, message); }
    static void warn(const std::string& message) { log(LogLevel::WARN, message); }
    static void error(const std::string& message) { log(LogLevel::ERROR, message); }
};

// Static member definition
LogLevel Logger::currentLevel_ = LogLevel::INFO;

} // namespace utils