#include "file_handler.h"
#include <fstream>
#include <sstream>

std::optional<std::string> read_file(const std::string& path) {
    // Block directory traversal — "../" in a path is never legitimate here
    if (path.find("..") != std::string::npos) return std::nullopt;

    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) return std::nullopt;

    std::ostringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

std::string mime_type(const std::string& path) {
    auto dot = path.rfind('.');
    if (dot == std::string::npos) return "application/octet-stream";

    std::string ext = path.substr(dot);
    if (ext == ".html" || ext == ".htm") return "text/html";
    if (ext == ".css")                   return "text/css";
    if (ext == ".js")                    return "application/javascript";
    if (ext == ".json")                  return "application/json";
    if (ext == ".png")                   return "image/png";
    if (ext == ".jpg" || ext == ".jpeg") return "image/jpeg";
    if (ext == ".ico")                   return "image/x-icon";
    if (ext == ".txt")                   return "text/plain";

    return "application/octet-stream";
}