#include <cctype>
#include "RESPParser.h"


std::vector<std::string> RESPParser::parseRESP(const std::string& input) {
    std::vector<std::string> tokens{};
    int count_token = 0;
    int count = 0;
    int count_char_in_token = 0;
    std::string substring = "";

    for (size_t i = 0; i < input.size(); ++i) {
        if (std::isdigit(input[i])) {
            switch (status_) {
                case PARSE_STATUS::INIT:
                    status_ = PARSE_STATUS::NUMBER;
                    substring += input[i]; 
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
                    break;
                case PARSE_STATUS::DELIMITER:
                    if (substring.size() == count) {
                        if (count == 3) {
                            status_ = PARSE_STATUS::NUMBER;
                        }
                        else {
                            status_ = PARSE_STATUS::TOKEN;
                        }
                            
                        substring = input[i];
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
                        count_token = std::stoi(substring);
                        count = 2;
                    }
                    
                    status_ = PARSE_STATUS::DELIMITER;
                    count_char_in_token =  std::stoi(substring);
                    substring = input[i];
                    break;
                default: // status_ = PARSE_STATUS::TOKEN
                    if (substring.size() == count_char_in_token) {
                        tokens.push_back(substring);

                        status_ = PARSE_STATUS::DELIMITER;
                        substring = input[i];
                        count = 3;
                    }
                    else {
                        substring += input[i];
                    }
                    break;
            }
        }
    }
    if (substring.size() == count_token && (status_ == PARSE_STATUS::NUMBER || status_ == PARSE_STATUS::TOKEN)) {
        status_ = PARSE_STATUS::INIT;
    }
    return tokens;
}
    