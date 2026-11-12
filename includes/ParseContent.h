#pragma once

#include <vector>
#include <string>
#include <cstddef>


enum class PARSE_STATE {
    EXPECT_ARRAY_PREFIX,
    READ_ARRAY_LENGTH,
    EXPECT_CRLF,
    EXPECT_PAYLOAD_PREFIX,
    READ_PAYLOAD_LENGTH,
    READ_PAYLOAD_DATA
};


struct ParseContent {
    PARSE_STATE state = PARSE_STATE::EXPECT_ARRAY_PREFIX;
    std::vector<std::string> tokens{};
    std::size_t n_tokens = 0;
    std::size_t n_char_in_token = 0;
    std::string substring = "";
    std::size_t bytes_in_command = 0;
    bool pre_token_CRLF = false;
};