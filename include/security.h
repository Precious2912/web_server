#ifndef SECURITY_H
#define SECURITY_H

#include "request.h"
#include <string>

// All security checks return this — caller maps it to an HTTP response
enum class SecurityStatus {
    OK,
    BAD_REQUEST,    // malformed input
    FORBIDDEN,      // valid request but access denied
    NOT_FOUND,      // path doesn't exist inside www
    TOO_LARGE,      // body exceeded limit
};

// Validates the full request before it touches any business logic
SecurityStatus validate_request(const HttpRequest& req);

// Resolves and validates a URL path against the www root.
// Returns the safe absolute path on success, empty string if rejected.
std::string resolve_safe_path(const std::string& url_path, const std::string& www_root);

// Maps a SecurityStatus to a ready-to-send HTTP response string
std::string security_error_response(SecurityStatus status);

#endif