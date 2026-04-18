#include <unordered_map>
#include "response.h"

static std::string status_text(int code) {
    static const std::unordered_map<int, std::string> texts = {
        {200, "OK"},
        {303, "See Other"},
        {400, "Bad Request"},
        {403, "Forbidden"},
        {404, "Not Found"},
        {405, "Method Not Allowed"},
        {413, "Payload Too Large"},
        {415, "Unsupported Media Type"},
        {500, "Internal Server Error"}

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


RouteResult response_bad_request() {
    return { 400, build_response(400, "text/plain", "400 Bad Request\n") };
}

RouteResult response_forbidden() {
    return { 403, build_response(403, "text/plain", "403 Forbidden\n") };
}

RouteResult response_not_found() {
    return { 404, build_response(404, "text/plain", "404 Not Found\n") };
}

RouteResult response_method_not_supported() {
    return { 405, build_response(405, "text/plain", "405 Method Not Allowed\n") };
}

RouteResult response_payload_too_large() {
    return { 413, build_response(413, "text/plain", "413 Payload Too Large\n") };
}

RouteResult response_unsupported_media_type() {
    return { 415, build_response(415, "text/plain", "415 Unsupported Media Type\n") };
}

RouteResult response_redirect(const std::string& location) {
    return { 303, build_response(303, "text/plain", "See Other\n", {{"Location", location}}) };
}

RouteResult response_internal_server_error() {
    return { 500, build_response(500, "text/plain", "500 Internal Server Error\n") };
}