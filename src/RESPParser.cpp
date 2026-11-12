#include <cctype>
#include <charconv>
#include "RESPParser.h"


ParseResult RESPParser::parseRESP(const std::string& input, ParseContent& pc) {
    for (size_t i = 0; i < input.size(); ++i) {
        if (pc.bytes_in_command == resp_limits::MAX_COMMAND_BYTES)
            return {PARSE_STATUS::ERROR, 0};
        
        ++pc.bytes_in_command;

        char c = input[i];
        switch(pc.state) {
            case PARSE_STATE::EXPECT_ARRAY_PREFIX:
                if (c == '*') {
                    pc.state = PARSE_STATE::READ_ARRAY_LENGTH;
                }
                else {
                    return {PARSE_STATUS::ERROR, i};
                }
                break;
            case PARSE_STATE::READ_ARRAY_LENGTH:
            case PARSE_STATE::READ_PAYLOAD_LENGTH:
                if (std::isdigit(static_cast<unsigned char>(c))) {
                    pc.substring += c;
                }
                else if (c == '\r' && pc.substring.size() != 0) {
                    std::size_t length{};
                    auto [ptr, ec] = std::from_chars(
                        pc.substring.data(),
                        pc.substring.data() + pc.substring.size(),
                        length
                    );

                    if (ec != std::errc{} || ptr != pc.substring.data() + pc.substring.size()) {
                        return {PARSE_STATUS::ERROR, i};
                    }

                    if (pc.state == PARSE_STATE::READ_ARRAY_LENGTH) {
                        if (length <= resp_limits::MAX_PAYLOAD_NUMBER) {
                            pc.n_tokens = length;
                        }
                        else {
                            return {PARSE_STATUS::ERROR, i};
                        }
                    }
                    else {
                        if (length <= resp_limits::MAX_PAYLOAD_SIZE) {
                            pc.n_char_in_token = length;
                        }
                        else {
                            return {PARSE_STATUS::ERROR, i};
                        }
                    }
                    pc.state = PARSE_STATE::EXPECT_CRLF;
                    pc.substring = "";
                }
                else {
                    return {PARSE_STATUS::ERROR, i};
                }
                break;
            case PARSE_STATE::EXPECT_CRLF:
                if (c == '\n') {
                    pc.state = pc.pre_token_CRLF ? PARSE_STATE::READ_PAYLOAD_DATA : PARSE_STATE::EXPECT_PAYLOAD_PREFIX;
                    pc.pre_token_CRLF = !pc.pre_token_CRLF;

                    if (pc.tokens.size() == pc.n_tokens) {
                        return {PARSE_STATUS::COMPLETE, i + 1};
                    }
                    else if (pc.tokens.size() != pc.n_tokens && i == input.size() - 1) {
                        if (pc.bytes_in_command == resp_limits::MAX_COMMAND_BYTES)
                            return {PARSE_STATUS::ERROR, 0};
                        
                        return {PARSE_STATUS::INCOMPLETE, input.size()};
                    }
                }
                else {
                    return {PARSE_STATUS::ERROR, i};
                }
                break;
            case PARSE_STATE::EXPECT_PAYLOAD_PREFIX:
                if (c == '$') {
                    pc.state = PARSE_STATE::READ_PAYLOAD_LENGTH;
                }
                else {
                    return {PARSE_STATUS::ERROR, i};
                }
                break;
            case PARSE_STATE::READ_PAYLOAD_DATA:
                if (pc.substring.size() < pc.n_char_in_token) {
                    pc.substring += c;
                }
                else if (pc.substring.size() == pc.n_char_in_token && c == '\r') {
                    pc.tokens.push_back(pc.substring);
                    pc.state = PARSE_STATE::EXPECT_CRLF;
                    pc.substring = "";
                }
                else {
                    return {PARSE_STATUS::ERROR, i};
                }
                break;
        }
    }
    
    if (pc.bytes_in_command == resp_limits::MAX_COMMAND_BYTES)
        return {PARSE_STATUS::ERROR, input.size()};

    return {PARSE_STATUS::INCOMPLETE, input.size()};
}