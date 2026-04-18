#ifndef CONNECTION_HANDLER_H
#define CONNECTION_HANDLER_H

#include <string>

// Handles one accepted connection end-to-end: read, parse, route, respond.
// Takes ownership of client_fd — the ClientSocket inside will close it.
// Called from a thread pool worker.
void handle_connection(int client_fd, const std::string& client_ip);

#endif