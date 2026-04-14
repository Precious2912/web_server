#include "socket.h"
#include <cstring>
#include <stdexcept>

static const size_t INITIAL_BUF = 512;
static const size_t MAX_HEADER  = 8192; // 8KB hard cap

ServerSocket::ServerSocket(int port) {
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) throw std::runtime_error("Socket creation failed");

    int opt = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // Clients that connect and go silent will block recv() forever without this
    struct timeval timeout;
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;
    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    if (bind(sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0)
        throw std::runtime_error("Bind failed");

    if (listen(sockfd, 10) < 0)
        throw std::runtime_error("Listen failed");
}

int ServerSocket::accept_client() const {
    return accept(sockfd, nullptr, nullptr);
}

void ClientSocket::send_response(const std::string& response) const {
    send(sockfd, response.c_str(), response.size(), 0);
}

std::string ClientSocket::receive_request() const {
    std::string buffer;
    buffer.reserve(INITIAL_BUF);

    char chunk[INITIAL_BUF];

    while (true) {
        ssize_t bytes = recv(sockfd, chunk, sizeof(chunk), 0);

        if (bytes <= 0) break; // timeout, disconnect, or error

        buffer.append(chunk, bytes);

        // Enforce the cap before we accept more
        if (buffer.size() > MAX_HEADER) break;

        // HTTP headers end at \r\n\r\n — stop reading once we have them
        if (buffer.find("\r\n\r\n") != std::string::npos) break;
    }

    return buffer;
}