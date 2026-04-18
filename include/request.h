#ifndef REQUEST_H
#define REQUEST_H

#include <string>
#include <unordered_map>

struct HttpRequest {
    std::string method;
    std::string path;
    std::string version;
    std::unordered_map<std::string, std::string> headers;
    std::string body; // Added for POST requests
    bool valid = false; // Any request that failed to parse
};

HttpRequest parse_request(const std::string& raw);

#endif