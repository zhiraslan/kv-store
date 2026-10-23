#pragma once

#include <string>


class TCPServer {
public:
    TCPServer(std::string ip_addr, int port);
    TCPServer(const TCPServer&) = delete;
    TCPServer& operator=(const TCPServer&) = delete;
    
    ~TCPServer();

    void run();
private:
    bool openListener();
    void acceptLoop();
    void handleClient(int client_fd);
    void closeListener();

    std::string ip_ = "";
    int port_ = 0;
    int listen_fd_ = -1;
};