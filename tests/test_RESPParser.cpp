#include <gtest/gtest.h>
#include <string>
#include "RESPParser.h"
#include "ParseContent.h"


TEST(RESPParser, CompleteBulk) {
    RESPParser parser;
    ParseContent content{};

    std::string input = "*3\r\n$3\r\nSET\r\n$5\r\n\nuser\r\n$5\r\nAlice\r\n";
    const std::vector<std::string> expected {"SET", "\nuser", "Alice"};
    
    ParseResult res = parser.parseRESP(input, content);
    
    EXPECT_EQ(res.status, PARSE_STATUS::COMPLETE);
    EXPECT_EQ(content.tokens, expected);
}

TEST(RESPParser, IncompleteBulk) {
    RESPParser parser;
    ParseContent content{};

    std::string input1 = "*3\r\n$3\r\nSET\r\n$4\r\nuser\r\n$5\r\nAlice\r";
    const std::vector<std::string> expected {"SET", "user", "Alice"};
    ParseResult res = parser.parseRESP(input1, content);
    EXPECT_EQ(res.status, PARSE_STATUS::INCOMPLETE);
    EXPECT_EQ(content.tokens, expected);


    content = {};
    std::string input2 = "*3\r\n$3\r\n";
    ParseResult res2 = parser.parseRESP(input2, content);
    EXPECT_EQ(res2.status, PARSE_STATUS::INCOMPLETE);
}

TEST(RESPParser, EmptyPayload) {
    RESPParser parser;
    ParseContent content{};

    std::string input = "*3\r\n$3\r\nSET\r\n$4\r\n\r\n$5\r\nAlice\r";
    const std::vector<std::string> expected {"SET", "user", "Alice"};
    
    ParseResult res = parser.parseRESP(input, content);
    
    EXPECT_EQ(res.status, PARSE_STATUS::ERROR);
}

TEST(RESPParser, InvalidBulk) {
    RESPParser parser;
    
    ParseContent content{};
    std::string input1 = "*2\r\n$3\r\nSET\r\n$5\r\n\r\n$5\r\n";
    ParseResult res = parser.parseRESP(input1, content);
    EXPECT_EQ(res.status, PARSE_STATUS::ERROR);   
}