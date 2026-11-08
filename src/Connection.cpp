#include <cerrno>
#include <unistd.h>
#include <arpa/inet.h>
#include <cstdio>
#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <cstddef>
#include "Connection.h"
#include "RESPParser.h"
#include "CommandProcessor.h"


 Connection::Connection(int client_fd, sockaddr_in client_addr)
    : client_fd_{client_fd}
    , port_{ntohs(client_addr.sin_port)} {
    std::array<char, INET_ADDRSTRLEN> ip_buffer{};

    if (inet_ntop(AF_INET, &client_addr.sin_addr, ip_buffer.data(), static_cast<socklen_t>(ip_buffer.size())) == nullptr) {
        std::perror("inet_ntop");
        ip_ = "<unknown>";
    }
    else {
        ip_ = ip_buffer.data();
    }
  
    std::cout << "Client connected: " << ip_.data() << ':' << port_ << '\n';
}

Connection::~Connection() {
    if (close(client_fd_) == -1) {
        std::perror("client socket");
    }
}

void Connection::run() {
    RESPParser parser;
    std::array<char, 1024> buffer;
    CommandProcessor processor;

    while (true) {
        ssize_t bytes_arrived = recv(client_fd_, buffer.data(), buffer.size(), 0);

        if (bytes_arrived > 0) {
            std::cout.write(buffer.data(), static_cast<std::streamsize>(bytes_arrived));
            std::cout.flush();
        }
        else if (bytes_arrived == 0) {
            std::cout << "Client disconnected: " << ip_ << ':' << port_ << '\n';
            break;
        }
        else if (bytes_arrived == -1) {
            if (errno == EINTR) // errno - code of the last system error in the current stream
                continue;
            
            std::perror("recv");
            break; 
        }

        std::string chunk(buffer.data(), bytes_arrived);
        ParseResult result = parser.parseRESP(chunk);

        if (result.status == PARSE_STATUS::COMPLETE) {
            std::string reply = processor.execute(result.tokens);
            
            if (!sendAll(reply))
                break;

            parser.reset();
        }
        else if (result.status == PARSE_STATUS::ERROR) {
            parser.reset();            
        }
        else if (result.status == PARSE_STATUS::INCOMPLETE)
            continue;
    }
}

bool Connection::sendAll(const std::string& reply) {
    size_t sent_total = 0;

    while (sent_total < reply.size()) {
        ssize_t sent = send(client_fd_, reply.data() + sent_total, reply.size() - sent_total, MSG_NOSIGNAL);

        if (sent > 0) {
            sent_total += static_cast<std::size_t>(sent);
        }
        else if (sent == -1 && errno == EINTR) {
            continue;
        }
        else {
            std::perror("send");
            return false;
        }
    }

    return true;
}