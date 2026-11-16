#pragma once


class Socket {
public:
    Socket(int fd);

    Socket(const Socket&) = delete;
    Socket& operator=(const Socket&) = delete;
    Socket(Socket&& other) noexcept;
    
    ~Socket();
private:
    int socket_ = -1;
};