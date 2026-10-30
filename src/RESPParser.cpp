#include <cctype>
#include "RESPParser.h"


std::vector<std::string> RESPParser::parseRESP(const std::string& input) {
    std::vector<std::string> tokens{};
    int count_token = 0;
    int count = 0;
    int count_char_in_token = 0;
    std::string substring = "";
    bool should_stop = false;

    for (size_t i = 0; i < input.size(); ++i) {
        if (std::isdigit(input[i])) {
            switch (status_) {
                case PARSE_STATUS::INIT:
                    if (i == 1) {
                        status_ = PARSE_STATUS::NUMBER;
                        substring += input[i];
                    }
                    else {
                        should_stop = true;
                        status_ = PARSE_STATUS::INVALID;
                    }                    
                    break;
                case PARSE_STATUS::DELIMITER:
                    status_ = PARSE_STATUS::NUMBER;
                    substring = input[i]; 
                    break;
                case PARSE_STATUS::NUMBER:
                    substring += input[i];
                    break;
                default: // status_ = PARSE_STATUS::TOKEN
                    substring += input[i];
                    break;
            }
        }
        else {
            switch (status_) {
                case PARSE_STATUS::INIT:
                    if (input[i] != '*') {
                        should_stop = true;
                        status_ = PARSE_STATUS::INVALID;
                    }

                    break;
                case PARSE_STATUS::DELIMITER:
                    if (substring.size() == count) {
                        if (count == 3 && substring == "\r\n$") {
                            status_ = PARSE_STATUS::NUMBER;
                            substring = input[i];
                        }
                        else if (substring == "\r\n") {
                            status_ = PARSE_STATUS::TOKEN;
                            substring = input[i];
                        }
                        else {
                            should_stop = true;
                            status_ = PARSE_STATUS::INVALID;
                        }                        
                    }
                    else {
                        substring += input[i];
                    }
                    break;
                case PARSE_STATUS::NUMBER:
                    if (!count_token) {
                        count_token = std::stoi(substring);
                        count = 3;
                    }
                    else {
                        count_char_in_token =  std::stoi(substring);
                        count = 2;
                    }
                    
                    status_ = PARSE_STATUS::DELIMITER;
                    substring = input[i];
                    break;
                default: // status_ = PARSE_STATUS::TOKEN
                    if (substring.size() == count_char_in_token) {
                        tokens.push_back(substring);

                        status_ = PARSE_STATUS::DELIMITER;
                        substring = input[i];
                        count = tokens.size() == count_token ? 2 : 3;
                    }
                    else {
                        substring += input[i];
                    }
                    break;
            }
        }

        if (should_stop) {
            break;
        }
    }
    if (tokens.size() == count_token && substring == "\r\n" && status_ == PARSE_STATUS::DELIMITER) {
        status_ = PARSE_STATUS::INIT;
    }
    return tokens;
}


PARSE_STATUS RESPParser::getStatus() const {
    return static_cast<PARSE_STATUS>(status_);
}