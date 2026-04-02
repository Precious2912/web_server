#ifndef SOCKET_H
#define SOCKET_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdexcept>
#include <string>

// RAII Base Socket Class
class Socket {
protected:
    int sockfd;

public:
    Socket() : sockfd(-1) {}
    explicit Socket(int fd) : sockfd(fd) {}
    
    // Ensures the socket gets closed when object dies
    virtual ~Socket() {
        if (sockfd != -1) {
            close(sockfd);
        }
    }

    // Disable copying - prevents double-close bugs
    Socket(const Socket&) = delete;
    Socket& operator=(const Socket&) = delete;

    int get_fd() const { return sockfd; }
};

class ServerSocket : public Socket {
public:
    ServerSocket(int port);
    int accept_client() const;
};

class ClientSocket : public Socket {
public:
    explicit ClientSocket(int fd) : Socket(fd) {}
    void send_response(const std::string& response) const;
    std::string receive_request() const;
};

#endif