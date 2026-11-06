#include <cctype>
#include "RESPParser.h"


ParseResult RESPParser::parseRESP(const std::string& input) {
    for (size_t i = 0; i < input.size(); ++i) {
        char c = input[i];
        switch(state_) {
            case PARSE_STATE::EXPECT_ARRAY_PREFIX:
                if (c == '*') {
                    state_ = PARSE_STATE::READ_ARRAY_LENGTH;
                }
                else {
                    status_ = PARSE_STATUS::ERROR;
                    tokens_ = {};
                    return {status_, tokens_};
                }
                break;
            case PARSE_STATE::READ_ARRAY_LENGTH:
            case PARSE_STATE::READ_PAYLOAD_LENGTH:
                if (std::isdigit(static_cast<unsigned char>(c))) {
                    substring += c;
                }
                else if (c == '\r' && substring.size() != 0) {
                    if (state_ == PARSE_STATE::READ_ARRAY_LENGTH) {
                        n_tokens = std::stoi(substring);  // number of tokens
                    }
                    else {
                        n_char_in_token = std::stoi(substring);
                    }
                    state_ = PARSE_STATE::EXPECT_CRLF;
                    substring = "";
                }
                else {
                    status_ = PARSE_STATUS::ERROR;
                    tokens_ = {};
                    return {status_, tokens_};
                }
                break;
            case PARSE_STATE::EXPECT_CRLF:
                if (c == '\n') {
                    state_ = pre_token_CRLF ? PARSE_STATE::READ_PAYLOAD_DATA : PARSE_STATE::EXPECT_PAYLOAD_PREFIX;
                    pre_token_CRLF = !pre_token_CRLF;

                    if (i == input.size() - 1) {
                        if (tokens_.size() == n_tokens) {status_ = PARSE_STATUS::COMPLETE;}
                        else {
                            status_ = PARSE_STATUS::INCOMPLETE;
                            return {status_, tokens_};
                        }
                    }
                }
                else {
                    status_ = PARSE_STATUS::ERROR;
                    tokens_ = {};
                    return {status_, tokens_};
                }
                break;
            case PARSE_STATE::EXPECT_PAYLOAD_PREFIX:
                if (c == '$') {
                    state_ = PARSE_STATE::READ_PAYLOAD_LENGTH;
                }
                else {
                    status_ = PARSE_STATUS::ERROR;
                    tokens_ = {};
                    return {status_, tokens_};
                }
                break;
            case PARSE_STATE::READ_PAYLOAD_DATA:
                if (substring.size() < n_char_in_token) {substring += c;}
                else if (substring.size() == n_char_in_token && c == '\r') {
                    tokens_.push_back(substring);
                    state_ = PARSE_STATE::EXPECT_CRLF;
                    substring = "";
                }
                else {
                    status_ = PARSE_STATUS::ERROR;
                    tokens_ = {};
                    return {status_, tokens_};
                }
                break;
        }
    }
    
    return {status_, tokens_};
}


PARSE_STATUS RESPParser::getStatus() const {
    return status_;
}

void RESPParser::reset() {
    state_ = PARSE_STATE::EXPECT_ARRAY_PREFIX;
    status_ = PARSE_STATUS::INCOMPLETE;
    tokens_ = {};
    n_tokens = 0;
    n_char_in_token = 0;
    substring = "";
    pre_token_CRLF = false;
}