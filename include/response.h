#ifndef RESPONSE_H
#define RESPONSE_H

#include <string>

// Builds a complete HTTP response string ready to send down the wire
std::string build_response(int status_code,
                           const std::string& content_type,
                           const std::string& body);

// Convenience wrappers so call sites don't repeat themselves
std::string response_bad_request();
std::string response_not_found();

#endif