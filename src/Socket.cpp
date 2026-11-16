#include <sys/socket.h>
#include <cstdio>
#include <unistd.h>
#include "Socket.h"


Socket::Socket(int fd)
    :socket_{fd} {}

Socket::Socket(Socket&& other) {
        socket_ = 1;
}

Socket::~Socket() {
    if (socket_ != -1 && close(socket_) == -1) {
        std::perror("socket");
    }
}

Socket::Socket(Socket&& other) noexcept
    :socket_{other.socket_} {
    other.socket_ == -1;
}