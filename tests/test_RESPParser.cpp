#include <gtest/gtest.h>
#include <string>
#include <vector>
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

TEST(RESPParser, InvalidBulk) {
    RESPParser parser;
    
    ParseContent content{};
    std::string input1 = "*2\r\n$3\r\nSET\r\n$5\r\n\r\n$5\r\n";
    ParseResult res = parser.parseRESP(input1, content);
    EXPECT_EQ(res.status, PARSE_STATUS::ERROR);

    content = {};
    std::string input2 = "*3\r\n$3\r\nSET\r\n$4\r\n\r\n$5\r\nAlice\r";
    res = parser.parseRESP(input2, content);
    EXPECT_EQ(res.status, PARSE_STATUS::ERROR);
}

TEST(RESPParser, EmptyPayload) {
    RESPParser parser;
    ParseContent content{};

    std::string input = "*3\r\n$3\r\nSET\r\n$0\r\n\r\n$1\r\nx\r\n";
    const std::vector<std::string> expected {"SET", "", "x"};
    
    ParseResult res = parser.parseRESP(input, content);
    
    EXPECT_EQ(res.status, PARSE_STATUS::COMPLETE);
    EXPECT_EQ(content.tokens, expected);
}

TEST(RESPParser, ResponseWhenMultipleCommands) {
    RESPParser parser;
    ParseContent content{};

    std::string input = "*1\r\n$3\r\nCMD\r\n*2\r\n$3\r\nSET\r\n$3\r\nkey\r\n";
    
    ParseResult res = parser.parseRESP(input, content);
    
    ASSERT_EQ(res.status, PARSE_STATUS::COMPLETE);
    ASSERT_EQ(res.bytes_parsed, 13);
    ASSERT_EQ(content.tokens, std::vector<std::string>{"CMD"});
    
    content = {};
    input = input.substr(res.bytes_parsed);
    const std::vector<std::string> expected{"SET", "key"};
    
    res = parser.parseRESP(input, content);
    
    EXPECT_EQ(res.status, PARSE_STATUS::COMPLETE);
    EXPECT_EQ(res.bytes_parsed, input.size());
    EXPECT_EQ(content.tokens, expected);
    EXPECT_EQ(res.bytes_parsed, input.size());
}

TEST(RESPParser, IncompleteAfterComplete) {
    RESPParser parser;
    ParseContent content{};

    std::string input1 = "*3\r\n$3\r\nSET\r\n$9\r\nzhiraslan\r\n$8\r\nshedzhem\r\n*2\r\n$3\r\nGET\r\n$";
    const std::vector<std::string> expected1{"SET", "zhiraslan", "shedzhem"};
    ParseResult res = parser.parseRESP(input1, content);
    
    ASSERT_EQ(res.status, PARSE_STATUS::COMPLETE);
    EXPECT_EQ(content.tokens, expected1);

    content = {};
    std::string input2 = input1.substr(res.bytes_parsed);
    res = parser.parseRESP(input2, content);
    
    EXPECT_EQ(res.status, PARSE_STATUS::INCOMPLETE);

    std::string input3 = "9\r\nzhiraslan\r\n";
    const std::vector<std::string> expected2{"GET", "zhiraslan"};
    res = parser.parseRESP(input3, content);
    
    EXPECT_EQ(res.status, PARSE_STATUS::COMPLETE);
    EXPECT_EQ(content.tokens, expected2);
}

TEST(RESPParser, InvalidCRLF) {
    RESPParser parser;
    ParseContent content{};

    std::string input = "*1\rx$4\r\nPING\r\n";    
    ParseResult res = parser.parseRESP(input, content);
    
    ASSERT_EQ(res.status, PARSE_STATUS::ERROR);

    content = {};
    input = "*1\r\n$4\rxPING\r\n";
    res = parser.parseRESP(input, content);
    EXPECT_EQ(res.status, PARSE_STATUS::ERROR);

    content = {};
    input = "*1\r\n$4\r\nPING\rx";
    res = parser.parseRESP(input, content);
    EXPECT_EQ(res.status, PARSE_STATUS::ERROR);
}

TEST(RESPParser, NonDigitPayloadSize) {
    RESPParser parser;
    ParseContent content{};

    std::string input = "*1\r\n$x\r\nPING\r\n";    
    ParseResult res = parser.parseRESP(input, content);
    
    ASSERT_EQ(res.status, PARSE_STATUS::ERROR);
}

TEST(RESPParser, PayloadSizeOverhead) {
    RESPParser parser;
    ParseContent content{};

    std::string input = "*1\r\n$18446744073709551616\r\n";    
    ParseResult res = parser.parseRESP(input, content);
    
    ASSERT_EQ(res.status, PARSE_STATUS::ERROR);

    content = {};
    input = "*1\r\n$1048577\r\n";
    res = parser.parseRESP(input, content);
    EXPECT_EQ(res.status, PARSE_STATUS::ERROR);
}

TEST(RESPParser, AcceptsMaximumPayloadSize) {
    RESPParser parser;
    ParseContent content{};

    const std::string payload(resp_limits::MAX_PAYLOAD_SIZE, 'a');

    std::string input = "*1\r\n$";
    input += std::to_string(resp_limits::MAX_PAYLOAD_SIZE);
    input += "\r\n";
    input.append(payload);
    input += "\r\n";

    const ParseResult res = parser.parseRESP(input, content);

    ASSERT_EQ(res.status, PARSE_STATUS::COMPLETE);
    EXPECT_EQ(res.bytes_parsed, input.size());
    ASSERT_EQ(content.tokens.size(), 1);
    EXPECT_EQ(content.tokens[0], payload);
}

TEST(RESPParser, RejectsTooManyArrayElements) {
    RESPParser parser;
    ParseContent content{};

    std::string input = "*";
    input += std::to_string(resp_limits::MAX_PAYLOAD_NUMBER + 1);
    input += "\r\n";

    const ParseResult res = parser.parseRESP(input, content);

    EXPECT_EQ(res.status, PARSE_STATUS::ERROR);
}

TEST(RESPParser, AcceptsCommandAtMaximumBytes) {
    RESPParser parser;
    ParseContent content{};

    constexpr std::size_t payload_size = 838'849;

    std::string input = "*5\r\n";

    input += "$838849\r\n";
    input.append(payload_size, 'a');
    input += "\r\n";

    input += "$838849\r\n";
    input.append(payload_size, 'a');
    input += "\r\n";

    input += "$838849\r\n";
    input.append(payload_size, 'a');
    input += "\r\n";

    input += "$838849\r\n";
    input.append(payload_size, 'a');
    input += "\r\n";

    input += "$838849\r\n";
    input.append(payload_size, 'a');
    input += "\r\n";

    ASSERT_EQ(input.size(), resp_limits::MAX_COMMAND_BYTES);

    const ParseResult res = parser.parseRESP(input, content);

    EXPECT_EQ(res.status, PARSE_STATUS::COMPLETE);
    EXPECT_EQ(res.bytes_parsed, resp_limits::MAX_COMMAND_BYTES);
}

TEST(RESPParser, RejectsNonNumericPayloadLength) {
    RESPParser parser;
    ParseContent content{};

    const std::string input = "*1\r\n$x\r\n";

    const ParseResult res = parser.parseRESP(input, content);

    EXPECT_EQ(res.status, PARSE_STATUS::ERROR);
}

TEST(RESPParser, RejectsNonNumericArrayLength) {
    RESPParser parser;
    ParseContent content{};

    const std::string input = "*x\r\n";

    const ParseResult res = parser.parseRESP(input, content);

    EXPECT_EQ(res.status, PARSE_STATUS::ERROR);
}

TEST(RESPParser, EmptyArray) {
    RESPParser parser;
    ParseContent content{};

    const std::string input = "*0\r\n";

    const ParseResult res = parser.parseRESP(input, content);

    ASSERT_EQ(res.status, PARSE_STATUS::COMPLETE);
    EXPECT_TRUE(content.tokens.empty());
    EXPECT_EQ(res.bytes_parsed, input.size());
}

TEST(RESPParser, BinaryPayload) {
    RESPParser parser;
    ParseContent content{};

    const std::string payload{"a\0\r\nb", 5};

    std::string input = "*1\r\n$5\r\n";
    input.append(payload);
    input += "\r\n";

    const ParseResult res = parser.parseRESP(input, content);

    ASSERT_EQ(res.status, PARSE_STATUS::COMPLETE);
    ASSERT_EQ(content.tokens.size(), 1);
    EXPECT_EQ(content.tokens[0], payload);
}

TEST(RESPParser, AcceptsMaximumArrayLength) {
    RESPParser parser;
    ParseContent content{};

    std::string input = "*";
    input += std::to_string(resp_limits::MAX_PAYLOAD_NUMBER);
    input += "\r\n";

    for (std::size_t i = 0; i < resp_limits::MAX_PAYLOAD_NUMBER; ++i) {
        input += "$0\r\n\r\n";
    }

    const ParseResult res = parser.parseRESP(input, content);

    ASSERT_EQ(res.status, PARSE_STATUS::COMPLETE);
    EXPECT_EQ(content.tokens.size(), resp_limits::MAX_PAYLOAD_NUMBER);
    EXPECT_EQ(res.bytes_parsed, input.size());
}