#include "router.h"
#include "response.h"
#include "file_handler.h"
#include "form_handler.h"
#include "security.h"
#include "utils.h"

static const std::string WWW_ROOT = "./www";

// The IPC fd is set once at startup by main() after the fork.
// Router uses it to forward POST bodies to the form handler process.
static int g_ipc_fd = -1;

void set_ipc_fd(int fd) {
    g_ipc_fd = fd;
}


// Pulls Content-Length from headers, returns -1 if missing or invalid
static long get_content_length(const HttpRequest& req) {
    auto it = req.headers.find("content-length");
    if (it == req.headers.end()) return -1;

    std::string val = it->second;

    // Trim trailing whitespace — some clients include \r or spaces
    auto end = val.find_last_not_of(" \t\r\n");
    if (end != std::string::npos)
        val = val.substr(0, end + 1);

    try {
        long len = std::stol(val);
        return (len >= 0) ? len : -1;
    } catch (const std::exception&) {
        return -1; // non-numeric value
    }
}

static bool is_form_encoded(const HttpRequest& req) {
    auto it = req.headers.find("content-type");
    if (it == req.headers.end()) return false;

    // Allow "application/x-www-form-urlencoded; charset=UTF-8" etc.
    return it->second.find("application/x-www-form-urlencoded") == 0;
}

static std::string handle_get(const HttpRequest& req) {
    std::string path = req.path;
    if (path == "/") path = "/index.html";

    // resolve_safe_path handles realpath + root confinement
    std::string safe_path = resolve_safe_path(path, WWW_ROOT);
    if (safe_path.empty())
        return security_error_response(SecurityStatus::FORBIDDEN);

    auto contents = read_file(safe_path);
    if (!contents) return security_error_response(SecurityStatus::NOT_FOUND);

    return build_response(200, mime_type(WWW_ROOT + path), *contents);
}

static std::string handle_post(const HttpRequest& req) {
    if (req.path != "/submit") 
        return response_not_found();
        
    if (!is_form_encoded(req))
        return response_unsupported_media_type();

    // Content-Length is mandatory for POST — no guessing body size
    long content_length = get_content_length(req);
    if (content_length < 0) 
        return response_bad_request();

    // Reject before we even look at the body
    if (static_cast<size_t>(content_length) > MAX_BODY_SIZE)
        return build_response(413, "text/plain", "413 Payload Too Large");

    // Reject if body shorter than promised
    if (static_cast<long>(req.body.size()) < content_length)
        return response_bad_request();

    // Slice exactly Content-Length bytes — don't trust anything after
    if (req.body.size() > static_cast<size_t>(content_length))
        return response_bad_request();

    std::string body = req.body.substr(0, content_length);

    // auto form_data = parse_form_body(body);
    // if (!form_data) return response_bad_request();

    // if (!save_form_data(*form_data))
    //     return response_internal_server_error();

    // Hand off to the isolated form handler process rather than processing
    // it here — keeps file writes out of the main server process entirely
    if (g_ipc_fd < 0 || !send_to_form_handler(g_ipc_fd, body))
        return response_internal_server_error();

    // Redirect back to the form so refreshing doesn't resubmit
    return response_redirect("/form.html");
}

std::string route(const HttpRequest& req) {
    // Every request passes through the security layer first
    SecurityStatus status = validate_request(req);
    if (status != SecurityStatus::OK)
        return security_error_response(status);

    if (req.method == "GET")  return handle_get(req);
    if (req.method == "POST") return handle_post(req);

    return response_method_not_supported();
}