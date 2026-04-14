#ifndef REQUEST_H
#define REQUEST_H

#include <string>
#include <unordered_map>

struct HttpRequest {
    std::string method;
    std::string path;
    std::string version;
    std::unordered_map<std::string, std::string> headers;
    bool valid = false; // Any request that failed to parse
};

HttpRequest parse(const std::string& raw);

#endif