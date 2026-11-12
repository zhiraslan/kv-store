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
            if (content_.state == PARSE_STATE::EXPECT_ARRAY_PREFIX) {
                std::cout << "Client disconnected: " << ip_ << ':' << port_ << '\n';
            }
            else {
                std::cout << "Client disconnected: " << ip_ << ':' << port_ << " before completing the request.\n";
            }
            break;
        }
        else if (bytes_arrived == -1) {
            if (errno == EINTR) // errno - code of the last system error in the current stream
                continue;
            
            std::perror("recv");
            break;
        }

        std::string chunk(buffer.data(), bytes_arrived);
        std::vector<std::string> replies{};
        bool protocol_error = false;

        while (chunk.size()) {
            ParseResult parse_result = parser.parseRESP(chunk, content_);
            
            if (parse_result.status == PARSE_STATUS::COMPLETE) {
                replies.push_back(processor.execute(content_.tokens));
                chunk = chunk.substr(parse_result.bytes_parsed);
                reset();
            }
            else if (parse_result.status == PARSE_STATUS::INCOMPLETE) {
                break;
            }
            else if (parse_result.status == PARSE_STATUS::ERROR) {
                replies.push_back(std::string("-ERR Protocol error\r\n"));
                protocol_error = true;
                break;
            }
        }

        if (!sendAll(replies))
            return;

        if (protocol_error)
            return;
    }
}


bool Connection::sendAll(const std::vector<std::string>& replies) {
    for (const std::string& reply : replies) {
        size_t sent_total = 0;

        while (sent_total < reply.size()) {
            ssize_t sent = send(
                client_fd_,
                reply.data() + sent_total,
                reply.size() - sent_total,
                MSG_NOSIGNAL
            );

            if (sent > 0) {
                sent_total += static_cast<std::size_t>(sent);
            }
            else if (sent == -1 && errno == EINTR) {
                continue;
            }
            else if (sent == 0) {
                std::cerr << "Send returned 0; no data was sent to client.\n";
                return false;
            }
            else {
                std::perror("send");
                return false;
            }
        }
    }

    return true;
}

void Connection::reset() {
    content_.state = PARSE_STATE::EXPECT_ARRAY_PREFIX;
    content_.tokens = {};
    content_.n_tokens = 0;
    content_.n_char_in_token = 0;
    content_.substring = "";
    content_.pre_token_CRLF = false;
    content_.bytes_in_command = 0;
}


