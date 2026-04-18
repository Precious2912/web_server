#include "connection_handler.h"
#include "logger.h"
#include "request.h"
#include "response.h"
#include "router.h"
#include "socket.h"

static std::string request_log_line(
    const std::string& ip,
    const HttpRequest& req,
    int status) 
{
    return ip + " " + req.method + " " + req.path + " -> " + std::to_string(status);
}

void handle_connection(int client_fd, const std::string& client_ip) {
    ClientSocket client(client_fd);

    try {
        std::string raw = client.receive_request();

        if (raw.empty()) {
            LOG_WARN(client_ip + " -> empty request, closing connection");
            return;
        }
        
        HttpRequest req = parse_request(raw);

        if (!req.valid) {
            LOG_WARN(client_ip + " -> 400 malformed request");
            client.send_response(response_bad_request().response);
            return;
        }

        RouteResult result = route(req);
        client.send_response(result.response);

        std::string log_line = request_log_line(client_ip, req, result.status);

        if (result.status >= 500) { LOG_ERROR(log_line); }
        else if (result.status >= 400) { LOG_WARN(log_line);  }
        else { LOG_INFO(log_line);  }

        return;

    } catch (const std::exception& e) {
        LOG_ERROR(client_ip + " unhandled exception: " + e.what());
    }
}