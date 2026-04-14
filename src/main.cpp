#include "socket.h"
#include "router.h"
#include "ipc.h"
#include "form_handler.h"
#include "thread_pool.h"
#include "connection_handler.h"
#include "logger.h"
#include <filesystem>
#include <iostream>

static const int PORT = 8080;
static const size_t NUM_THREADS = 4;
static const char*  LOG_PATH    = "./logs/sss.log";

int main() {
    try {
        std::filesystem::create_directories("./logs");

        Logger logger(LOG_PATH);
        g_logger = &logger;
        
        // IPC channel set up before fork - both processes inherit both ends,
        // then each closes the end it doesn't own
        IpcChannel ipc = create_ipc_channel();

        // Automatically reap the child when it exits — no zombie processes
        signal(SIGCHLD, SIG_IGN);

        // If the form handler dies, writes to the pipe return an error
        // instead of delivering SIGPIPE and killing the whole server
        signal(SIGPIPE, SIG_IGN);

        pid_t pid = fork();
        if (pid < 0) {
            throw std::runtime_error("fork failed");
        }

        if (pid == 0) {
            // Child process — form handler only 
            // Close the parent's end — child only talks on child_fd
            close(ipc.parent_fd);
            form_handler_loop(ipc.child_fd);
            return 0; // should only reach here if parent closes the channel
        }

        // Parent process — HTTP server 
        // Close the child's end — parent only talks on parent_fd
        close(ipc.child_fd);

        // Wire up the router so POST requests know where to send bodies
        set_ipc_fd(ipc.parent_fd);

        ServerSocket server(PORT);
        ThreadPool pool(NUM_THREADS);

        LOG_INFO("SSS started | port=" + std::to_string(PORT) +
                 " | threads=" + std::to_string(NUM_THREADS) +
                 " | form_handler_pid=" + std::to_string(pid));

        std::cout << "SSS running on port " << PORT
                  << " | threads: "          << NUM_THREADS
                  << " | form handler pid: " << pid
                  << std::endl;


        while (true) {
            AcceptedClient ac = server.accept_client();
            if (ac.fd < 0) continue;

            // Captured by value — the lambda owns client_fd from this point.
            // handle_connection wraps it in a ClientSocket which closes it.
            pool.enqueue([ac]() {
                handle_connection(ac.fd, ac.ip);
            });
        }
    } catch (const std::exception& e) {
        LOG_ERROR("Fatal: " + std::string(e.what()));
        std::cerr << "Fatal: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}