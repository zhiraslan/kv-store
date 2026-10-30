#pragma once

#include <string>
#include <vector>


enum PARSE_STATUS {
    INIT,
    NUMBER,
    TOKEN,
    DELIMITER,
    INVALID
};


class RESPParser {
public:
    std::vector<std::string> parseRESP(const std::string& input);

    PARSE_STATUS getStatus() const;
private:
    PARSE_STATUS status_ = PARSE_STATUS::INIT;
};