#include <cctype>
#include <algorithm>
#include <optional>
#include "CommandProcessor.h"
#include "Service.h"


std::string CommandProcessor::execute(const std::vector<std::string>& tokens) {
    Service* service = Service::instance();
    COMMAND_TYPE type = COMMAND_TYPE::UNKNOWN;
    const std::string CRLF = "\r\n";
    std::string reply = "";

    if (tokens.empty())
        return "-ERR empty command" + CRLF;
    std::string command = tokens[0];
    
    
    std::transform(
        command.begin(),
        command.end(), 
        command.begin(),
        [](unsigned char c) {
            return static_cast<char>(std::tolower(c));
        }
    );        

    if (command == "set")
        type = COMMAND_TYPE::SET;
    else if (command == "get")
        type = COMMAND_TYPE::GET;
    else if (command == "del")
        type = COMMAND_TYPE::DEL;
    else
        return tokens.size() == 1
            ? "-ERR unknown command '" + tokens[0] + "'" + CRLF
            : "-ERR unknown command '" + tokens[0] + "', with args beginning with: '" + tokens[1] + "'" + CRLF;
    
    std::optional<std::string> value = std::nullopt;

    switch(type) {
        case COMMAND_TYPE::SET:
            if (tokens.size() != 3)
                return "-ERR wrong number of arguments for '" + command + "' command" + CRLF;
                
            service->set(tokens[1], tokens[2]);
            reply = "+OK" + CRLF;
            break;
        case COMMAND_TYPE::GET:
            if (tokens.size() != 2)
                return "-ERR wrong number of arguments for '" + command + "' command" + CRLF;

            value = service->get(tokens[1]);
            reply = value.has_value() ? "$" + std::to_string(value->size()) + CRLF + *value + CRLF : "$-1" + CRLF;
            break;
        case COMMAND_TYPE::DEL: {
            if (tokens.size() < 2)
                return "-ERR wrong number of arguments for '" + command + "' command" + CRLF;
            
            std::vector<std::string> keys(tokens.begin() + 1, tokens.end());
            std::size_t n_keys_deleted = service->del(keys);
            reply = ":" + std::to_string(n_keys_deleted) + CRLF;
            break;
        }
    };

    return reply;
}