#pragma once

#include <vector>
#include <string>


enum class COMMAND_TYPE {
    SET,
    GET,
    DEL,
    PING,
    UNKNOWN
};


struct Command {
    Command(const std::vector<std::string>& tokens)
        : name{tokens[0]}
        , args{std::vector(tokens.begin() + 1, tokens.end())} {}
   
    std::string name;
    std::vector<std::string> args;
};


class CommandProcessor {
public:
    std::string execute(const Command& cmd);
private:

};
