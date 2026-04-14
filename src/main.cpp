#include "socket.h"
#include "request.h"
#include "response.h"
#include "router.h"
#include "ipc.h"
#include "form_handler.h"
#include <iostream>

int main() {
    try {
        // Set up the IPC channel before forking — both processes inherit
        // the file descriptors, then each closes the end it doesn't need
        IpcChannel ipc = create_ipc_channel();

        pid_t pid = fork();
        if (pid < 0) {
            throw std::runtime_error("fork failed");
        }

        if (pid == 0) {
            // Child process — form handler only 
            // Close the parent's end — child only talks on child_fd
            close(ipc.parent_fd);
            form_handler_loop(ipc.child_fd);
            return 0; // should only reach here if parent closes the channel
        }

        // Parent process — HTTP server 
        // Close the child's end — parent only talks on parent_fd
        close(ipc.child_fd);

        // Wire up the router so POST requests know where to send bodies
        set_ipc_fd(ipc.parent_fd);

        const int PORT = 8080;
        ServerSocket server(PORT);
        //std::cout << "SSS running on port " << PORT << std::endl;
        std::cout << "SSS running on port " << PORT
                  << " (form handler pid: " << pid << ")" << std::endl;


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