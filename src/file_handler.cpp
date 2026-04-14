#include "file_handler.h"
#include <fstream>
#include <sstream>

bool read_file(const std::string& path, std::string& contents) {
    if (path.find("..") != std::string::npos) return false;

    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) return false;

    std::ostringstream ss;
    ss << file.rdbuf();
    contents = ss.str();
    return true;
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