#ifndef ROUTER_H
#define ROUTER_H

#include "request.h"
#include <string>

// Takes a parsed request, returns a ready-to-send HTTP response string
std::string route(const HttpRequest& req);

#endif