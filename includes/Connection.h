#pragma once

#include<array>
#include<netinet/in.h>
#include<string>


class Connection {
public:
    Connection(int client_fd, sockaddr_in client_addr);
    ~Connection();
    
    void run();
private:
    bool sendAll(const std::string& reply);

    int client_fd_ = -1;
    std::string ip_{};
    int port_{};

    std::array<char, 1024> buffer_{};    
};