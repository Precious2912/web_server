#ifndef RESPONSE_H
#define RESPONSE_H

#include <string>
#include <unordered_map>
#include "result.h"

// Builds a complete HTTP response string
std::string build_response(
    int status_code,
    const std::string& content_type,
    const std::string& body,
    const std::unordered_map<std::string, std::string>& extra_headers = {}
);

RouteResult response_bad_request();
RouteResult response_forbidden();
RouteResult response_not_found();
RouteResult response_method_not_supported();
RouteResult response_payload_too_large();
RouteResult response_unsupported_media_type();
RouteResult response_too_many_requests();
RouteResult response_redirect(const std::string& location);
RouteResult response_internal_server_error();

#endif