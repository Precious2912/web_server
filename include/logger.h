#ifndef LOGGER_H
#define LOGGER_H

#include <fstream>
#include <mutex>
#include <string>

enum class LogLevel {
    INFO,
    WARN,
    ERROR   // ERROR conflicts with a macro on some platforms
};

class Logger {
public:
    // Opens (or creates) the log file in append mode.
    // Throws if the file can't be opened — logging failure at startup is fatal.
    explicit Logger(const std::string& filepath);

    // Disable copying — one logger owns the file handle
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    void info (const std::string& message);
    void warn (const std::string& message);
    void error(const std::string& message);

private:
    std::ofstream file;
    std::mutex    write_mutex;

    void write(LogLevel level, const std::string& message);

    static std::string level_string(LogLevel level);
    static std::string timestamp();
};

// Single global instance — initialised in main() before fork(),
// inherited by the child process. Append-mode writes are safe across
// both processes on Linux without additional locking.
extern Logger* g_logger;

// Null-check wrappers so call sites don't have to think about it
#define LOG_INFO(msg)  do { if (g_logger) g_logger->info(msg);  } while(0)
#define LOG_WARN(msg)  do { if (g_logger) g_logger->warn(msg);  } while(0)
#define LOG_ERROR(msg) do { if (g_logger) g_logger->error(msg); } while(0)

#endif