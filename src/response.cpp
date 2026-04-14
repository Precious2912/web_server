#include "response.h"
#include <unordered_map>

static std::string status_text(int code) {
    static const std::unordered_map<int, std::string> texts = {
        {200, "OK"},
        {400, "Bad Request"},
        {404, "Not Found"},
    };
    auto it = texts.find(code);
    return it != texts.end() ? it->second : "Unknown";
}

std::string build_response(int status_code,
                           const std::string& content_type,
                           const std::string& body) {
    return "HTTP/1.1 " + std::to_string(status_code) + " " + status_text(status_code) + "\r\n"
           "Content-Type: "   + content_type + "\r\n"
           "Content-Length: " + std::to_string(body.size()) + "\r\n"
           "Connection: close\r\n"
           "\r\n"
           + body;
}

std::string response_bad_request() {
    return build_response(400, "text/plain", "400 Bad Request");
}

std::string response_not_found() {
    return build_response(404, "text/plain", "404 Not Found");
}