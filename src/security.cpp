#include "security.h"
#include "response.h"
#include "utils.h"
#include <sys/syslimits.h>
#include <cstdlib>
#include <filesystem>

static const std::string ALLOWED_METHODS[] = { "GET", "POST" };
static const size_t MAX_METHOD_LEN  = 7;
static const size_t MAX_PATH_LEN    = 2048;

static bool has_null_byte(const std::string& s) {
    return s.find('\0') != std::string::npos;
}

static bool has_traversal(const std::string& s) {
    return s.find("..") != std::string::npos;
}

static bool is_allowed_method(const std::string& method) {
    for (const auto& m : ALLOWED_METHODS)
        if (method == m) return true;
    return false;
}

int security_status_code(SecurityStatus status) {
    switch (status) {
        case SecurityStatus::FORBIDDEN:          return 403;
        case SecurityStatus::NOT_FOUND:          return 404;
        case SecurityStatus::METHOD_NOT_ALLOWED: return 405;
        case SecurityStatus::TOO_LARGE:          return 413;
        default:                                 return 400;
    }
}

RouteResult security_error_response(SecurityStatus status) {
    switch (status) {
        case SecurityStatus::FORBIDDEN:          return response_forbidden();
        case SecurityStatus::NOT_FOUND:          return response_not_found();
        case SecurityStatus::METHOD_NOT_ALLOWED: return response_method_not_supported();
        case SecurityStatus::TOO_LARGE:          return response_payload_too_large();
        default:                                 return response_bad_request();
    }
}

std::string resolve_safe_path(const std::string& url_path, const std::string& www_root) {
    std::string decoded = url_decode(url_path);
    if (decoded.empty()) return "";

    // Fast reject before touching the filesystem
    if (has_null_byte(decoded)) return "";
    if (has_traversal(decoded)) return "";

    char resolved_root[PATH_MAX];
    if (!realpath(www_root.c_str(), resolved_root)) return "";
    std::string root_str(resolved_root);

    // Trailing slash matters — without it, /www-evil would pass as /www
    if (root_str.back() != '/') root_str += '/';

    char resolved_candidate[PATH_MAX];
    std::string candidate = www_root + decoded;
    if (!realpath(candidate.c_str(), resolved_candidate)) return "";
    std::string candidate_str(resolved_candidate);

    if (candidate_str.back() != '/') candidate_str += '/';

    // rfind at 0 = prefix check. Explicit and hard to misread.
    if (candidate_str.rfind(root_str, 0) != 0) return "";

    // Symlink note: realpath() follows them, so a symlink pointing outside
    // /www will fail the prefix check above. One inside /www is fine.
    // If policy changes, lstat() here can block all of them.

    candidate_str.pop_back();
    return candidate_str;
}

SecurityStatus validate_request(const HttpRequest& req) {
    if (req.method.empty() || req.method.size() > MAX_METHOD_LEN)
        return SecurityStatus::BAD_REQUEST;

    if (!is_allowed_method(req.method))
        return SecurityStatus::METHOD_NOT_ALLOWED;

    if (req.path.empty() || req.path.size() > MAX_PATH_LEN)
        return SecurityStatus::BAD_REQUEST;

    if (has_null_byte(req.path))
        return SecurityStatus::BAD_REQUEST;

    // Decode before checking — catches encoded bypasses
    std::string decoded = url_decode(req.path);
    if (decoded.empty())
        return SecurityStatus::BAD_REQUEST;

    if (has_traversal(decoded))
        return SecurityStatus::FORBIDDEN;

    size_t header_size = 0;
    for (const auto& [key, value] : req.headers) {
        header_size += key.size() + value.size();
        if (header_size > MAX_HEADER_SIZE)
            return SecurityStatus::BAD_REQUEST;
    }

    if (req.method == "POST") {
        auto it = req.headers.find("content-length");
        if (it == req.headers.end())
            return SecurityStatus::BAD_REQUEST;

        try {
            long len = std::stol(it->second);
            if (len < 0) return SecurityStatus::BAD_REQUEST;
            if (static_cast<size_t>(len) > MAX_BODY_SIZE)
                return SecurityStatus::TOO_LARGE;
        } catch (...) {
            return SecurityStatus::BAD_REQUEST;
        }
    }

    return SecurityStatus::OK;
}