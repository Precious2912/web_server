#ifndef ROUTER_H
#define ROUTER_H

#include "request.h"
#include "result.h"
#include <string>

// Called once at startup after the fork so the router knows
// where to send POST bodies
void set_ipc_fd(int fd);

// Takes a parsed request, returns a ready-to-send HTTP response string
RouteResult route(const HttpRequest& req);

#endif