#include <cerrno>
#include <unistd.h>
#include <arpa/inet.h>
#include <cstdio>
#include <iostream>
#include "Connection.h"


Connection::Connection(int client_fd, const std::array<char, INET_ADDRSTRLEN>& ip, int port)
    : client_fd_{client_fd}
    , ip_{ip}
    , port_{port}  {
        std::cout << "Client connected: " << ip_.data() << ':' << port_ << '\n';
    }

void Connection::run() {}