#include "socket.h"
#include "request.h"
#include "response.h"
#include "router.h"
#include <iostream>

int main() {
    try {
        const int PORT = 8080;
        ServerSocket server(PORT);
        std::cout << "SSS running on port " << PORT << std::endl;

        while (true) {
            int client_fd = server.accept_client();
            if (client_fd < 0) continue;

            ClientSocket client(client_fd);

            std::string raw = client.receive_request();

            // Empty read - just move on. Lol
            if (raw.empty()) continue;

            // Cap exceeded before we found \r\n\r\n — malformed or attack
            HttpRequest req = parse_request(raw);
            std::string response = req.valid ? route(req) : response_bad_request();

            client.send_response(response);
        }
    } catch (const std::exception& e) {
        std::cerr << "Fatal: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}