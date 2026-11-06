#pragma once

#include <string>
#include <vector>


enum class PARSE_STATE {
    EXPECT_ARRAY_PREFIX,
    READ_ARRAY_LENGTH,
    EXPECT_CRLF,
    EXPECT_PAYLOAD_PREFIX,
    READ_PAYLOAD_LENGTH,
    READ_PAYLOAD_DATA
};

enum class PARSE_STATUS {    
    COMPLETE,
    INCOMPLETE,
    ERROR
};

struct ParseResult {
    PARSE_STATUS status;
    std::vector<std::string> tokens;
};


class RESPParser {
public:
    ParseResult parseRESP(const std::string& input);
    void reset();
    PARSE_STATUS getStatus() const;

private:
    PARSE_STATE state_ = PARSE_STATE::EXPECT_ARRAY_PREFIX;
    PARSE_STATUS status_ = PARSE_STATUS::INCOMPLETE;
    std::vector<std::string> tokens_{};
    int n_tokens = 0;
    int n_char_in_token = 0;
    std::string substring = "";
    bool pre_token_CRLF = false;
};