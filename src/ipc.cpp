#include "ipc.h"
#include <sys/socket.h>
#include <unistd.h>
#include <stdexcept>

IpcChannel create_ipc_channel() {
    int fds[2];

    // socketpair gives us a bidirectional pipe between two processes —
    // cleaner than a regular pipe because we can use send/recv on both ends
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, fds) < 0)
        throw std::runtime_error("socketpair failed");

    return { fds[0], fds[1] };
}