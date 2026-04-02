SSS Secure Web Server Prototype - Compilation Instructions

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
configured port (default 8080).

CLEANING BUILD ARTIFACTS

To remove object files and the executable binary, run:

    make clean
