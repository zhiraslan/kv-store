#include <cctype>
#include <algorithm>
#include <optional>
#include "CommandProcessor.h"
#include "Service.h"


std::string CommandProcessor::execute(const Command& command) {
    Service* service = Service::instance();
    COMMAND_TYPE type = COMMAND_TYPE::UNKNOWN;
    const std::string CRLF = "\r\n";
    std::string reply = "";
    
    std::transform(
        command.name.begin(),
        command.name.end(), 
        command.name.begin(),
        [](unsigned char c) {
            return static_cast<char>(std::tolower(c));
        }
    );        

    if (command.name == "set")
        type = COMMAND_TYPE::SET;
    else if (command.name == "get")
        type = COMMAND_TYPE::GET;
    else if (command.name == "del")
        type = COMMAND_TYPE::DEL;
    else if (command.name == "ping")
        type = COMMAND_TYPE::PING;
    else
        return command.args.size() == 1
            ? "-ERR unknown command '" + command.name + "'" + CRLF
            : "-ERR unknown command '" + command.name + "', with args beginning with: '" + command.args[0] + "'" + CRLF;
    
    std::optional<std::string> value = std::nullopt;

    switch(type) {
        case COMMAND_TYPE::SET:
            if (command.args.size() != 3)
                return "-ERR wrong number of arguments for '" + command.name + "' command" + CRLF;
                
            service->set(command.args[1], command.args[2]);
            reply = "+OK" + CRLF;
            break;
        case COMMAND_TYPE::GET:
            if (command.args.size() != 2)
                return "-ERR wrong number of arguments for '" + command.name + "' command" + CRLF;

            value = service->get(command.args[1]);
            reply = value.has_value() ? "$" + std::to_string(value->size()) + CRLF + *value + CRLF : "$-1" + CRLF;
            break;
        case COMMAND_TYPE::DEL: {
            if (command.args.size() < 2)
                return "-ERR wrong number of arguments for '" + command.name + "' command" + CRLF;
            
            std::vector<std::string> keys(command.args.begin() + 1, command.args.end());
            std::size_t n_keys_deleted = service->del(keys);
            reply = ":" + std::to_string(n_keys_deleted) + CRLF;
            break;
        }
        case COMMAND_TYPE::PING:
            if (command.args.size() == 1)
                reply = "+PONG" + CRLF;
            else if (command.args.size() == 2)
                reply = "$" + std::to_string(command.args[1].size()) + CRLF + command.args[1] + CRLF;
            else
                reply = "-ERR wrong number of arguments for '" + command.name + "' command" + CRLF;
            break;
    };

    return reply;
}


using Response = std::string;
using Handler = Response (*)(const Command&);
Response executeSet(const Command& command) {
    const std::string CRLF = "\r\n";
    if (command.args.size() != 3)
        return "-ERR wrong number of arguments for '" + command.name + "' command" + CRLF;
    
    Service* service = Service::instance();
    service->set(command.args[1], command.args[2]);
    Response response = "+OK" + CRLF;
}