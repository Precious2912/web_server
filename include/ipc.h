#ifndef IPC_H
#define IPC_H

// File descriptors for the socketpair.
// Parent writes to fds[0], child reads from fds[1]
struct IpcChannel {
    int parent_fd; // parent keeps this end
    int child_fd;  // child keeps this end
};

// Creates the socketpair and returns both ends.
IpcChannel create_ipc_channel();

#endif