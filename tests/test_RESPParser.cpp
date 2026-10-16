#include <gtest/gtest.h>
#include <string>
#include <vector>
#include "RESPParser.h"


TEST(RESPParser, GetTokens) {
    RESPParser parser;
    // std::string input = "*3\r\n$3\r\nSET\r\n$4\r\nuser\r\n$5\r\nAlice\r\n";
    // const std::vector<std::string> expected{"SET", "user", "Alice"};

    std::string input = "*3\r\n$3\r\nSET\r\n$5\r\nuser1\r\n$6\r\nA1ice2\r\n";
    std::vector<std::string> expected{"SET", "user1", "A1ice2"};

    std::vector<std::string> res = parser.parseRESP(input);
    EXPECT_EQ(expected, res);
}