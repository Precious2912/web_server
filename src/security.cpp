#include "security.h"
#include "response.h"
#include "form_handler.h" // for MAX_BODY_SIZE
#include <climits>
#include <cstdlib>
#include <filesystem>

static const size_t MAX_HEADER_SIZE  = 8192;
static const size_t MAX_METHOD_LEN   = 8;   // longest we know is "DELETE" — anything over is sus
static const size_t MAX_PATH_LEN     = 2048;

// Allowed methods — expand this as steps add support
static const std::string ALLOWED_METHODS[] = { "GET", "POST" };

// ─── Helpers ──────────────────────────────────────────────────────────────────

static bool has_null_byte(const std::string& s) {
    return s.find('\0') != std::string::npos;
}

static bool has_traversal(const std::string& s) {
    // Catches both "../" and ".." at end of string
    return s.find("..") != std::string::npos;
}

static bool is_allowed_method(const std::string& method) {
    for (const auto& m : ALLOWED_METHODS) {
        if (method == m) return true;
    }
    return false;
}

// ─── Path Resolution ──────────────────────────────────────────────────────────

std::string resolve_safe_path(const std::string& url_path, const std::string& www_root) {
    // Null bytes in a path are always an attack
    if (has_null_byte(url_path)) return "";

    // Early traversal check before we even touch the filesystem
    if (has_traversal(url_path)) return "";

    // Resolve www_root to its real absolute path first
    char resolved_root[PATH_MAX];
    if (!realpath(www_root.c_str(), resolved_root)) return "";

    std::string root_str(resolved_root);

    // Build candidate path and resolve it
    std::string candidate = www_root + url_path;
    char resolved_candidate[PATH_MAX];

    if (!realpath(candidate.c_str(), resolved_candidate)) return "";

    std::string candidate_str(resolved_candidate);

    // The resolved file must live inside the resolved root — no exceptions.
    // The trailing slash on root_str ensures /www-evil doesn't pass as /www
    if (root_str.back() != '/') root_str += '/';

    if (candidate_str.substr(0, root_str.size()) != root_str) return "";

    return candidate_str;
}

// ─── Request Validation ───────────────────────────────────────────────────────

SecurityStatus validate_request(const HttpRequest& req) {
    // Method checks
    if (req.method.empty() || req.method.size() > MAX_METHOD_LEN)
        return SecurityStatus::BAD_REQUEST;

    if (!is_allowed_method(req.method))
        return SecurityStatus::BAD_REQUEST;

    // Path checks
    if (req.path.empty() || req.path.size() > MAX_PATH_LEN)
        return SecurityStatus::BAD_REQUEST;

    if (has_null_byte(req.path))
        return SecurityStatus::BAD_REQUEST;

    if (has_traversal(req.path))
        return SecurityStatus::FORBIDDEN;

    // Header size — rough check on total header footprint
    size_t header_size = 0;
    for (const auto& [key, value] : req.headers) {
        header_size += key.size() + value.size();
        if (header_size > MAX_HEADER_SIZE)
            return SecurityStatus::BAD_REQUEST;
    }

    // Body size enforcement for POST
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

// ─── Error Mapping ────────────────────────────────────────────────────────────

std::string security_error_response(SecurityStatus status) {
    switch (status) {
        case SecurityStatus::BAD_REQUEST:
            return build_response(400, "text/plain", "400 Bad Request");
        case SecurityStatus::FORBIDDEN:
            return build_response(403, "text/plain", "403 Forbidden");
        case SecurityStatus::NOT_FOUND:
            return build_response(404, "text/plain", "404 Not Found");
        case SecurityStatus::TOO_LARGE:
            return build_response(413, "text/plain", "413 Payload Too Large");
        default:
            return build_response(400, "text/plain", "400 Bad Request");
    }
}