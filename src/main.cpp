#include "socket.h"
#include <iostream>

int main() {
    try {
        const int PORT = 8080;
        ServerSocket server(PORT);
        std::cout << "SSS Secure Server prototype running on port " << PORT << "..." << std::endl;

        while (true) {
            int client_fd = server.accept_client();
            if (client_fd < 0) continue;

            // ClientSocket RAII will close fd when it goes out of scope
            ClientSocket client(client_fd);
            
            // Hardcoded HTTP 200 response
            std::string response = 
                "HTTP/1.1 200 OK\r\n"
                "Content-Type: text/html\r\n"
                "Content-Length: 24\r\n"
                "Connection: close\r\n"
                "\r\n"
                "<h1>SSS Prototype</h1>";
            
            client.send_response(response);
        }
    } catch (const std::exception& e) {
        std::cerr << "Fatal Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}