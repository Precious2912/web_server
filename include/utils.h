#ifndef UTILS_H
#define UTILS_H

#include <string>

// Shared size limits
static const size_t MAX_HEADER_SIZE = 8192;        // 8KB  — request headers
static const size_t MAX_BODY_SIZE   = 1024 * 1024; // 1MB  — request body

// Shared helpers
std::string url_decode(const std::string& s);

#endif