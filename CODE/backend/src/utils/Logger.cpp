#include "utils/Logger.h"
#include <ctime>

std::mutex Logger::logMutex;

std::string Logger::levelToString(Level level) {
    switch (level) {
        case Level::INFO: return "INFO";
        case Level::ERROR: return "ERROR";
        case Level::DEBUG: return "DEBUG";
        case Level::WARN: return "WARN";
        default: return "LOG";
    }
}

std::string Logger::currentDateTime() {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

void Logger::log(Level level, const std::string& msg) {
    std::lock_guard<std::mutex> lock(logMutex);
    std::cout << "[" << currentDateTime() << "] [" << levelToString(level) << "] " << msg << std::endl;
}

void Logger::info(const std::string& msg) {
    log(Level::INFO, msg);
}
void Logger::error(const std::string& msg) {
    log(Level::ERROR, msg);
}
void Logger::debug(const std::string& msg) {
    log(Level::DEBUG, msg);
}
void Logger::warn(const std::string& msg) {
    log(Level::WARN, msg);
}


