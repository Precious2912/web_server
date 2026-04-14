#ifndef RESULT_H
#define RESULT_H

#include <string>

// A completed HTTP result — the status code and the ready-to-send
// response string kept together so callers never have to re-derive
// one from the other.
struct RouteResult {
    int         status;
    std::string response;
};

#endif