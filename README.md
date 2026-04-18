Compilation Instructions

DEPENDENCIES
No third-party libraries are required. This implementation uses only:

C++17 standard library
POSIX system interfaces (sys/socket.h, unistd.h, arpa/inet.h) - part of the GNU C Library (glibc) version 2.37 (GNU libc 2.37, Fedora Linux)

COMPILER REQUIREMENT
g++ with C++17 support (GCC 8 or later)
Tested on GCC 13.2.1 (Red Hat 13.2.1-1) on Fedora Linux.

COMPILATION

To compile the secure web server natively on the Fedora VM, navigate to the project root directory and execute:

    make

This will use the provided Makefile to compile the source code using
g++ with security-hardened flags (-fstack-protector-strong and
-D_FORTIFY_SOURCE=2).

EXECUTABLE

The compilation process generates an executable binary named:
sss_server

RUNNING THE SERVER

To start the prototype server, run:

    ./sss_server

The server will begin listening for HTTP GET/POST requests on the
configured port (default 8080). Files served from ./www/. Submissions saved to ./data/. Logs written to ./logs/sss_server.log.

CLEANING BUILD ARTIFACTS

To remove object files and the executable binary, run:

    make clean
