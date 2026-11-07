#pragma once

#include <vector>
#include <string>


enum class COMMAND_TYPE {
    SET,
    GET,
    DEL,
    UNKNOWN
};


class CommandProcessor {
public:
    std::string execute(const std::vector<std::string>& tokens);
private:

};
