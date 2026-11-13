#pragma once

#include <array>
#include <netinet/in.h>
#include <string>
#include <vector>
#include "ParseContent.h"


class Connection {
public:
    Connection(int client_fd, sockaddr_in client_addr);
    Connection(const Connection&) = delete;
    Connection& operator=(const Connection&) = delete;
    ~Connection();
    
    void run();    
private:
    bool sendAll(const std::vector<std::string>& replies);
    void reset();

    ParseContent content_;
    int client_fd_ = -1;
    std::string ip_{};
    int port_{};
};