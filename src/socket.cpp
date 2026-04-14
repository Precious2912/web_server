#include "socket.h"
#include "utils.h"
#include <stdexcept>

static const size_t INITIAL_BUF = 512; // starting recv buffer size

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

// Case-insensitive scan for a header value in the raw buffer.
// Returns the index just past the colon, or npos if not found.
static size_t find_header_value(const std::string& buffer, const std::string& header_lc) {
    std::string lower_buf = buffer;
    std::transform(lower_buf.begin(), lower_buf.end(), lower_buf.begin(), ::tolower);

    auto pos = lower_buf.find(header_lc + ":");
    if (pos == std::string::npos) return std::string::npos;
    return pos + header_lc.size() + 1;
}

std::string ClientSocket::receive_request() const {
    std::string buffer;
    buffer.reserve(INITIAL_BUF);

    char chunk[INITIAL_BUF];

    // Read until we have the full headers
    while (buffer.find("\r\n\r\n") == std::string::npos) {
        ssize_t bytes = recv(sockfd, chunk, sizeof(chunk), 0);
        if (bytes <= 0) return buffer;

        buffer.append(chunk, bytes);

        if (buffer.size() > MAX_HEADER_SIZE) return buffer; // cap hit, parser rejects it
    }

    // Check if there's a body to read (Content-Length header present)
    auto cl_pos = find_header_value(buffer, "content-length");
    if (cl_pos != std::string::npos) {
        auto end   = buffer.find("\r\n", cl_pos);
        if (end != std::string::npos) {
            try {
                size_t content_length = std::stoul(buffer.substr(cl_pos, end - cl_pos));

                // Find where body starts
                auto body_start = buffer.find("\r\n\r\n") + 4;
                size_t body_have = buffer.size() - body_start;

                // Cap check before reading more
                if (content_length <= MAX_BODY_SIZE) {
                    while (body_have < content_length) {
                        ssize_t bytes = recv(sockfd, chunk, sizeof(chunk), 0);
                        if (bytes <= 0) break;
                        buffer.append(chunk, bytes);
                        body_have += bytes;
                    }
                }
            } catch (...) {
                // Non-numeric Content-Length — parser will reject it
            }
        }
    }

    return buffer;
}