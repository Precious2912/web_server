#include "response.h"
#include <unordered_map>

static std::string status_text(int code) {
    static const std::unordered_map<int, std::string> texts = {
        {200, "OK"},
        {303, "See Other"},
        {400, "Bad Request"},
        {403, "Forbidden"},
        {404, "Not Found"},
        {405, "Method Not Allowed"},
        {415, "Unsupported Media Type"},
        {500, "Internal Server Error"},
        //{413, "Payload Too Large"},

    };
    auto it = texts.find(code);
    return it != texts.end() ? it->second : "Unknown";
}

std::string build_response(int status_code,
                           const std::string& content_type,
                           const std::string& body,
                           const std::unordered_map<std::string, std::string>& extra_headers) {

    std::string response =
        "HTTP/1.1 " + std::to_string(status_code) + " " + status_text(status_code) + "\r\n"
        "Content-Type: " + content_type + "\r\n"
        "Content-Length: " + std::to_string(body.size()) + "\r\n"
        "Connection: close\r\n";

    for (const auto& [key, value] : extra_headers) {
        response += key + ": " + value + "\r\n";
    }

    response += "\r\n" + body;
    return response;
}


std::string response_bad_request() {
    return build_response(400, "text/plain", "400 Bad Request");
}

std::string response_not_found() {
    return build_response(404, "text/plain", "404 Not Found");
}

std::string response_method_not_supported() {
    return build_response(405, "text/plain", "405 Method Not Allowed");
}

std::string response_unsupported_media_type() {
    return build_response(415, "text/plain", "415 Unsupported Media Type");
}

std::string response_redirect(const std::string& location) {
    return build_response(303, "text/plain", "See Other", {{"Location", location}});
}

std::string response_internal_server_error() {
    return build_response(500, "text/plain", "500 Internal Server Error");
}