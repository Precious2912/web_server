#ifndef SOCKET_H
#define SOCKET_H

#include <netinet/in.h>
#include <stdexcept>
#include <string>
#include <sys/socket.h>
#include <unistd.h>

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

// Bundles the accepted fd with the client's IP string so the caller
// doesn't have to make a second call to get the address.
struct AcceptedClient {
    int         fd;
    std::string ip;
};

class ServerSocket : public Socket {
public:
    explicit ServerSocket(int port);
    // Returns the client fd and populates client_ip with the remote address
    //int accept_client(std::string& client_ip) const;
    AcceptedClient accept_client() const;

};

class ClientSocket : public Socket {
public:
    explicit ClientSocket(int fd) : Socket(fd) {}
    void send_response(const std::string& response) const;
    std::string receive_request() const;
};

#endif