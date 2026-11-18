#pragma once

#include <array>
#include <netinet/in.h>
#include <string>
#include "ParseContent.h"
#include "Socket.h"


class Connection {
public:
    Connection(Socket client_fd, sockaddr_in client_addr);
    Connection(const Connection&) = delete;
    Connection& operator=(const Connection&) = delete;
    
    void run();    
private:
    bool sendAll(const std::string& replies);
    void reset();

    ParseContent content_;
    Socket client_fd_{-1};
    std::string ip_{};
    int port_{};
};