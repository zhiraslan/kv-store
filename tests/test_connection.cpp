#include <gtest/gtest.h>
#include <cerrno>
#include <memory>
#include <string>
#include <thread>
#include <sys/socket.h>
#include <unistd.h>
#include "Connection.h"
#include "Socket.h"


class ConnectionTest : public ::testing::Test {
protected:
    int client_fd_ = -1;
    std::unique_ptr<Connection> connection_;
    std::thread worker_;

    void SetUp() override {
        int sockets[2];
        ASSERT_EQ(socketpair(AF_UNIX, SOCK_STREAM, 0, sockets), 0);

        client_fd_ = sockets[1];
        connection_ = std::make_unique<Connection>(
            Socket{sockets[0]},
            sockaddr_in{}
        );

        worker_ = std::thread([this] {
            connection_->run();
        });
    }

    void TearDown() override {
        if (client_fd_ != -1)
            shutdown(client_fd_, SHUT_WR);

        if (worker_.joinable())
            worker_.join();

        connection_.reset();

        if (client_fd_ != -1)
            close(client_fd_);
    }

    bool sendRequest(const std::string& request) {
        std::size_t sent_total = 0;

        while (sent_total < request.size()) {
            const ssize_t sent = send(
                client_fd_,
                request.data() + sent_total,
                request.size() - sent_total,
                MSG_NOSIGNAL
            );

            if (sent > 0) {
                sent_total += static_cast<std::size_t>(sent);
            }
            else if (sent == -1 && errno == EINTR) {
                continue;
            }
            else {
                return false;
            }
        }

        return true;
    }

    std::string finishAndReadResponse() {
        shutdown(client_fd_, SHUT_WR);

        if (worker_.joinable())
            worker_.join();

        // Connection владеет серверным сокетом.
        // Закрываем его, чтобы клиентская сторона получила EOF.
        connection_.reset();

        std::string response;
        char buffer[256];

        while (true) {
            const ssize_t received = recv(
                client_fd_,
                buffer,
                sizeof(buffer),
                0
            );

            if (received > 0) {
                response.append(buffer, static_cast<std::size_t>(received));
            }
            else if (received == -1 && errno == EINTR) {
                continue;
            }
            else {
                break;
            }
        }

        return response;
    }
};

TEST_F(ConnectionTest, Ping) {
    ASSERT_TRUE(sendRequest("*1\r\n$4\r\nPING\r\n"));

    EXPECT_EQ(finishAndReadResponse(), "+PONG\r\n");
}

TEST_F(ConnectionTest, PingWithArgument) {
    ASSERT_TRUE(sendRequest(
        "*2\r\n$4\r\nPING\r\n$5\r\nhello\r\n"
    ));

    EXPECT_EQ(finishAndReadResponse(), "$5\r\nhello\r\n");
}

TEST_F(ConnectionTest, PipelinedSetGetDel) {
    const std::string key = "connection-test:pipeline";
    const std::string request =
        "*3\r\n$3\r\nSET\r\n$" + std::to_string(key.size()) +
        "\r\n" + key + "\r\n$5\r\nvalue\r\n" +
        "*2\r\n$3\r\nGET\r\n$" + std::to_string(key.size()) +
        "\r\n" + key + "\r\n" +
        "*2\r\n$3\r\nDEL\r\n$" + std::to_string(key.size()) +
        "\r\n" + key + "\r\n";

    ASSERT_TRUE(sendRequest(request));

    EXPECT_EQ(
        finishAndReadResponse(),
        "+OK\r\n$5\r\nvalue\r\n:1\r\n"
    );
}

TEST_F(ConnectionTest, ProtocolError) {
    ASSERT_TRUE(sendRequest("*1\rx$4\r\nPING\r\n"));

    EXPECT_EQ(finishAndReadResponse(), "-ERR Protocol error\r\n");
}

TEST_F(ConnectionTest, IncompleteRequestGetsNoReply) {
    ASSERT_TRUE(sendRequest("*2\r\n$3\r\nGET\r\n$3\r\nke"));

    EXPECT_TRUE(finishAndReadResponse().empty());
}