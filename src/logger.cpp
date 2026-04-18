#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include "logger.h"

Logger* g_logger = nullptr;

Logger::Logger(const std::string& filepath) {
    file.open(filepath, std::ios::app);
    if (!file.is_open())
        throw std::runtime_error("Logger: could not open log file: " + filepath);
}

void Logger::info(const std::string& message) {
    write(LogLevel::INFO, message);
}

void Logger::warn(const std::string& message) {
    write(LogLevel::WARN, message);
}

void Logger::error(const std::string& message) {
    write(LogLevel::ERROR, message);
}

/* 
The mutex protects concurrent writes within the parent process (worker threads).
The child process has its own copy of the mutex after fork() — cross-process
write safety relies on O_APPEND atomicity for writes under PIPE_BUF bytes. 
*/ 
void Logger::write(LogLevel level, const std::string& message) {
    std::string line = "[" + timestamp() + "] [" + level_string(level) + "] " + message + "\n";

    {
        std::lock_guard<std::mutex> lock(write_mutex);
        file << line;
        file.flush();
    }

    // Mirror warnings and errors to stderr so they're visible during development
    if (level == LogLevel::WARN || level == LogLevel::ERROR)
        std::cerr << line;
        
}

std::string Logger::level_string(LogLevel level) {
    switch (level) {
        case LogLevel::INFO: return "INFO ";
        case LogLevel::WARN: return "WARN ";
        case LogLevel::ERROR: return "ERROR";
        default: return "?????";
    }
}

std::string Logger::timestamp() {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

    std::ostringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S")
       << '.' << std::setfill('0') << std::setw(3) << ms.count()
       << " BST";
    return ss.str();
}