#ifndef SECURITY_H
#define SECURITY_H

#include "request.h"
#include <string>

enum class SecurityStatus {
    OK,
    BAD_REQUEST,    // malformed input
    FORBIDDEN,      // valid request but access denied
    NOT_FOUND,      // path doesn't exist inside www
    METHOD_NOT_ALLOWED, // not get or post methods
    TOO_LARGE,      // body exceeded limit
};

SecurityStatus validate_request(const HttpRequest& req);

std::string resolve_safe_path(const std::string& url_path, const std::string& www_root);

std::string security_error_response(SecurityStatus status);

#endif