#ifndef RESPONSE_H
#define RESPONSE_H

#include <string>
#include <unordered_map>

// Builds a complete HTTP response string
std::string build_response(
    int status_code,
    const std::string& content_type,
    const std::string& body,
    const std::unordered_map<std::string, std::string>& extra_headers = {}
);

std::string response_bad_request();
std::string response_not_found();
std::string response_method_not_supported();
std::string response_unsupported_media_type();
std::string response_redirect(const std::string& location);
std::string response_internal_server_error();

#endif