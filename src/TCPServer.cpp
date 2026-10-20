#include <sys/types.h>
#include <sys/socket.h>
#include <iostream>
#include <cstdio>
#include <netinet/in.h>
#include <arpa/inet.h> // for inet_pton()
#include <unistd.h> // for close()
#include "TCPServer.h"


TCPServer::TCPServer(std::string ip_addr, int port)
        : ip_addr_(ip_addr)
        , port_(port) {}

TCPServer::~TCPServer() {
    closeListener();
}

void TCPServer::closeListener() {
    if (listen_fd_ != -1) {
        close(listen_fd_);
        listen_fd_ = -1;
    }
}

void TCPServer::run() {
    if (openListener()) {
        acceptLoop();
    }
}

bool TCPServer::openListener() {
    // creating a server listening socket
    listen_fd_ = socket(AF_INET, SOCK_STREAM, 0);

    if (listen_fd_ == -1) {
        std::perror("socket");
        return false;
    }
    // filling up the sockaddr struct
    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_port = htons(port_);

    int result = inet_pton(AF_INET, ip_addr_.c_str(), &address.sin_addr);

    if (!result) {
        std::cerr << "Invalid IPv4 address: " << ip_addr_ << ".\n";
        closeListener();
        return false;
    }
    else if (result == -1) {
        std::perror("inet_pton");
        closeListener();
        return false;
    }

    result = bind(listen_fd_, reinterpret_cast<sockaddr*>(&address), sizeof(address));

    if (result == -1) {
        std::perror("bind");
        closeListener();
        return false;
    }

    if (listen(listen_fd_, SOMAXCONN) == -1) {
        std::perror("listen");
        closeListener();
        return false;
    }

    return true;
}

void TCPServer::acceptLoop() {
    while (true) {
        int client_fd = accept(listen_fd_, nullptr, nullptr);  // to figure out the nullptrs

        if (client_fd == -1) {
            std::perror("accept");
            return;
        }

        handleClient(client_fd);
    }
}

void TCPServer::handleClient(int client_fd) {
    
}
