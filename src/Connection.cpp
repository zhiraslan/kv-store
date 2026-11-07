#include <cerrno>
#include <unistd.h>
#include <arpa/inet.h>
#include <cstdio>
#include <iostream>
#include <cctype>
#include <algorithm>
#include "Connection.h"
#include "RESPParser.h"
#include "Service.h"
#include "CommandProcessor.h"


Connection::Connection(int client_fd, const std::array<char, INET_ADDRSTRLEN>& ip, int port)
    : client_fd_{client_fd}
    , ip_{ip}
    , port_{port}  {
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
    Service* service = Service::instance();
    CommandProcessor processor;

    while (true) {
        ssize_t bytes_arrived = recv(client_fd_, buffer.data(), buffer.size(), 0);

        if (bytes_arrived > 0) {
            std::cout.write(buffer.data(), static_cast<std::streamsize>(bytes_arrived));
            std::cout.flush();
        }
        else if (bytes_arrived == 0) {
            std::cout << "Client disconnected: " << ip_.data() << ':' << ntohs(port_) << '\n';
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
        RESP_OPERATOR oper = RESP_OPERATOR::UNKNOWN;

        if (result.status == PARSE_STATUS::COMPLETE) {
            processor.execute(result.tokens);
            

            parser.reset();
        }
        else if (result.status == PARSE_STATUS::ERROR) {
            parser.reset();            
        }
        else if (result.status == PARSE_STATUS::INCOMPLETE)
            continue;
    }
}

