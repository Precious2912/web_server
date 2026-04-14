#ifndef SECURITY_H
#define SECURITY_H

#include "request.h"
#include "result.h"
#include <string>

enum class SecurityStatus {
    OK,
    BAD_REQUEST,    // malformed input
    FORBIDDEN,      // valid request but access denied
    NOT_FOUND,      // path doesn't exist inside www
    METHOD_NOT_ALLOWED, // not get or post methods
    TOO_LARGE,      // body exceeded limit
};

// Maps a SecurityStatus to its HTTP status code — single source of truth
// so router and security_error_response always agree on the code.
int security_status_code(SecurityStatus status);

SecurityStatus validate_request(const HttpRequest& req);

std::string resolve_safe_path(const std::string& url_path, const std::string& www_root);

RouteResult security_error_response(SecurityStatus status);

#endif