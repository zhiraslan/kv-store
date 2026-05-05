#pragma once
#include <csignal>
#include "Service.h"


class TcpServer {
public:
    TcpServer(int port, Service& service);
    void start();
    void stop();
private:
    int port_;
    Service& service_;
    int server_fd_;

    static volatile std::sig_atomic_t running;
};



