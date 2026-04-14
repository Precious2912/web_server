#include "request.h"
#include <sstream>

// Splits "Key: Value" into the map. Returns false if the line is malformed.
static bool parse_header_line(const std::string& line,
                               std::unordered_map<std::string, std::string>& headers) {
    auto colon = line.find(':');
    if (colon == std::string::npos) return false;

    std::string key   = line.substr(0, colon);
    std::string value = line.substr(colon + 1);

    // Trim leading whitespace from value — RFC 7230 allows it
    auto start = value.find_first_not_of(" \t");
    if (start != std::string::npos)
        value = value.substr(start);

    headers[key] = value;
    return true;
}

HttpRequest parse(const std::string& raw) {
    HttpRequest req;

    std::istringstream stream(raw);
    std::string request_line;

    if (!std::getline(stream, request_line)) return req;

    // Strip trailing \r if present (HTTP lines end in \r\n)
    if (!request_line.empty() && request_line.back() == '\r')
        request_line.pop_back();

    std::istringstream rl(request_line);
    if (!(rl >> req.method >> req.path >> req.version)) return req;

    if (req.method != "GET") return req;

    if (req.version != "HTTP/1.0" && req.version != "HTTP/1.1") return req;

    // Path must start with /
    if (req.path.empty() || req.path[0] != '/') return req;

    // Parse headers until blank line
    std::string line;
    while (std::getline(stream, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        if (line.empty()) break; // blank line = end of headers

        if (!parse_header_line(line, req.headers)) return req;
    }

    req.valid = true;
    return req;
}