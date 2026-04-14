#ifndef IPC_H
#define IPC_H

// File descriptors for the socketpair.
// Parent writes to fds[0], child reads from fds[1] — and vice versa.
// Both ends are bidirectional but we use them directionally.
struct IpcChannel {
    int parent_fd; // parent keeps this end
    int child_fd;  // child keeps this end
};

// Creates the socketpair and returns both ends.
// Throws on failure.
IpcChannel create_ipc_channel();

#endif