#include <algorithm>
#include <sstream>
#include "request.h"

static bool parse_header_line(
    const std::string& line,
    std::unordered_map<std::string, std::string>& headers) 
{
    auto colon = line.find(':');
    if (colon == std::string::npos) return false;

    std::string key = line.substr(0, colon);
    std::string value = line.substr(colon + 1);

    // Normalise to lowercase — HTTP headers are case-insensitive (RFC 7230)
    std::transform(key.begin(), key.end(), key.begin(), ::tolower);

    auto start = value.find_first_not_of(" \t");
    if (start != std::string::npos)
        value = value.substr(start);

    headers[key] = value;
    return true;
}

HttpRequest parse_request(const std::string& raw) {
    HttpRequest req;

    auto header_end = raw.find("\r\n\r\n");
    if (header_end == std::string::npos) return req;

    std::string header_section = raw.substr(0, header_end);
    req.body = raw.substr(header_end + 4);

    std::istringstream stream(header_section);
    std::string request_line;

    if (!std::getline(stream, request_line)) return req;

    if (!request_line.empty() && request_line.back() == '\r')
        request_line.pop_back();

    std::istringstream rl(request_line);
    if (!(rl >> req.method >> req.path >> req.version)) return req;

    if (req.version != "HTTP/1.0" && req.version != "HTTP/1.1") return req;

    if (req.path.empty() || req.path[0] != '/') return req;

    std::string line;
    while (std::getline(stream, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        if (line.empty()) break;
        if (!parse_header_line(line, req.headers)) return req;
    }

    req.valid = true;
    return req;
}