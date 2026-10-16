#pragma once

#include <string>
#include <vector>


enum PARSE_STATUS {
    INIT,
    NUMBER,
    TOKEN,
    DELIMITER
};


class RESPParser {
public:
    std::vector<std::string> parseRESP(const std::string& input);
private:
    bool first_char_ = true;
    PARSE_STATUS status_ = PARSE_STATUS::INIT;
};
