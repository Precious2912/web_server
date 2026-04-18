#include <stdexcept>
#include <sys/socket.h>
#include <unistd.h>
#include "ipc.h"

IpcChannel create_ipc_channel() {
    int fds[2];

    if (socketpair(AF_UNIX, SOCK_STREAM, 0, fds) < 0)
        throw std::runtime_error("socketpair failed");

    return { fds[0], fds[1] };
}