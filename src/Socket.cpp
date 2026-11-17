#include <sys/socket.h>
#include <cstdio>
#include <unistd.h>
#include "Socket.h"


Socket::Socket(int fd)
    :socket_{fd} {}

Socket::Socket(Socket&& other) noexcept
    : socket_{other.socket_} {
    other.socket_ = -1;
}

Socket& Socket::operator=(Socket&& other) noexcept {
    if (this != &other) {
        if (socket_ != -1 && close(socket_) == -1)
            std::perror("close");
    
        socket_ = other.socket_;
        other.socket_ = -1;
    }

    return *this;
}

Socket::~Socket() {
    if (socket_ != -1 && close(socket_) == -1) {
        std::perror("close");
    }
}

int Socket::get() const {
    return socket_;
}