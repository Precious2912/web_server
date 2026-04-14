#include "router.h"
#include "response.h"
#include "file_handler.h"

static const std::string WWW_ROOT = "./www";

std::string route(const HttpRequest& req) {
    std::string path = req.path;
    if (path == "/") path = "/index.html";

    std::string file_path = WWW_ROOT + path;
    std::string contents;

    if (!read_file(file_path, contents)) return response_not_found();

    return build_response(200, mime_type(file_path), contents);
}