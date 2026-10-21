#include <sys/types.h>
#include <sys/socket.h>
#include <iostream>
#include <cstdio>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <array>
#include <cerrno>
#include "TCPServer.h"


TCPServer::TCPServer(std::string ip_addr, int port)
        : ip_(ip_addr)
        , port_(port) {}

TCPServer::~TCPServer() {
    closeListener();
}

void TCPServer::closeListener() {
    if (listen_fd_ != -1) {
        if (close(listen_fd_) == -1) {
            std::perror("close listening socket");
        }
        
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

    int result = inet_pton(AF_INET, ip_.c_str(), &address.sin_addr);

    if (!result) {
        std::cerr << "Invalid IPv4 address: " << ip_ << ".\n";
        closeListener();
        return false;
    }
    else if (result == -1) {
        std::perror("inet_pton");
        closeListener();
        return false;
    }

    result = bind(listen_fd_, reinterpret_cast<sockaddr*>(&address), sizeof(address)); // to figure out reinterpret_cast

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
        sockaddr_in client_addr{};
        socklen_t address_size = sizeof(client_addr);

        int client_fd = accept(listen_fd_, reinterpret_cast<sockaddr*>(&client_addr), &address_size);  // to figure out the nullptrs

        if (client_fd == -1) {
            switch(errno) {
                case EINTR:
                case ECONNABORTED:
                case ENETDOWN:
                case EPROTO:
                case ENOPROTOOPT:
                case EHOSTDOWN:
                case ENONET:
                case EHOSTUNREACH:
                case EOPNOTSUPP:
                case ENETUNREACH:
                    continue;
                default:
                    std::perror("accept");
                    return;
            }            
        }

        std::array<char, INET_ADDRSTRLEN> client_ip;
        if (inet_ntop(AF_INET, &client_addr.sin_addr, client_ip.data(), client_ip.size()) == nullptr) {
            std::perror("inet_ntop");
            
            if (close(client_fd) == -1) {
                std::perror("close client");
            }
        
            continue;
        }

        std::cout << "Client connected: " << client_ip.data() << ':' << ntohs(client_addr.sin_port) << '\n';

        handleClient(client_fd);
    }
}

void TCPServer::handleClient(int client_fd) {
    sockaddr_in client_addr{};
    socklen_t client_address_size = sizeof(client_addr);
    std::array<char, INET_ADDRSTRLEN> client_ip{};

    if (getpeername(client_fd, reinterpret_cast<sockaddr*>(&client_addr), &client_address_size) == -1) {
        std::perror("getpeername");
        
        if (close(client_fd) == -1) {
            std::perror("close client");
        }
        
        return;
    }
    else if (inet_ntop(AF_INET, &client_addr.sin_addr, client_ip.data(), client_ip.size()) == nullptr) {
            std::perror("inet_ntop");

            if (close(client_fd) == -1) {
                std::perror("close client");
            }
        
            return;
    }    

    std::array<char, 1024> buffer;

    while (true) {
        ssize_t bytes_arrived = recv(client_fd, buffer.data(), buffer.size(), 0);

        if (bytes_arrived > 0) {
            std::cout.write(buffer.data(), static_cast<std::streamsize>(bytes_arrived));
            std::cout.flush();
        }
        else if (bytes_arrived == 0) {
            std::cout << "Client disconnected: " << client_ip.data() << ':' << ntohs(client_addr.sin_port) << '\n';
            break;
        }
        else if (bytes_arrived == -1) {
            if (errno == EINTR) // errno - code of the last system error in the current stream
                continue;
            
            std::perror("recv");
            break;
        }
    }
    
    if (close(client_fd) == -1) std::perror("close client");
}
