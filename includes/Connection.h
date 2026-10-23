#pragma once

#include<array>
#include<netinet/in.h>


class Connection {
public:
    Connection(int client_fd, const std::array<char, INET_ADDRSTRLEN>& ip, int port);
    void run();
private:
    int client_fd_ = -1;
    std::array<char, INET_ADDRSTRLEN> ip_{};
    int port_{};

    std::array<char, 1024> buffer_{};    
};