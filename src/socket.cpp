#include "socket.h"
#include <cstring>

ServerSocket::ServerSocket(int port) {
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) throw std::runtime_error("Socket creation failed");

    // SO_REUSEADDR to restart server instantly
    int opt = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // Security: Set receive timeout to prevent hanging connections 
    struct timeval timeout;
    timeout.tv_sec = 5; // 5 seconds
    timeout.tv_usec = 0;
    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    if (bind(sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        throw std::runtime_error("Bind failed");
    }

    if (listen(sockfd, 10) < 0) {
        throw std::runtime_error("Listen failed");
    }
}

int ServerSocket::accept_client() const {
    int client_fd = accept(sockfd, nullptr, nullptr);
    return client_fd;
}

void ClientSocket::send_response(const std::string& response) const {
    send(sockfd, response.c_str(), response.size(), 0);
}