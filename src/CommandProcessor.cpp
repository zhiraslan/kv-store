#include <cctype>
#include <algorithm>
#include <optional>
#include <unordered_map>
#include "Service.h"
#include "CommandProcessor.h"
#include "RESPReplies.h"
#include <string_view>


using Response = std::string;
using Handler = Response (*) (const Command& command);


template<typename... Parts>
std::string concatenate(const Parts&... parts) {
    std::string result;
    (result.append(std::string_view{parts}), ...);
    return result;
}

static std::string executeSet(const Command& command) {
    Service* service = Service::instance();

    if (command.args.size() != 2)
        return concatenate(resp::ERROR_ARGUMENTS_NUMBER, command.name, "' command", resp::CRLF);

    service->set(command.args[0], command.args[1]);
    return std::string{resp::OK};
}

static std::string executeGet(const Command& command) {
    Service* service = Service::instance();
    std::optional<std::string> value = std::nullopt;

    if (command.args.size() != 1)
        return concatenate(resp::ERROR_ARGUMENTS_NUMBER, command.name, "' command", resp::CRLF);

    value = service->get(command.args[0]);
    Response response = value.has_value()
        ? concatenate("$", std::to_string(value->size()), resp::CRLF, *value, resp::CRLF)
        : concatenate("$-1", resp::CRLF);
            
    return response;
}

static std::string executeDel(const Command& command) {
    Service* service = Service::instance();

    if (command.args.size() < 1)
        return concatenate(resp::ERROR_ARGUMENTS_NUMBER, command.name, "' command", resp::CRLF);
    
    std::vector<std::string> keys(command.args.begin(), command.args.end());
    std::size_t n_keys_deleted = service->del(keys);
    return concatenate(":", std::to_string(n_keys_deleted), resp::CRLF);
}

static std::string executePing(const Command& command) {
    Response response{};
    if (command.args.size() == 0)
        response = resp::PONG;
    else if (command.args.size() == 1)
        response = concatenate("$", std::to_string(command.args[0].size()), resp::CRLF, command.args[0], resp::CRLF);
    else
        return concatenate(resp::ERROR_ARGUMENTS_NUMBER, command.name, "' command", resp::CRLF);
    return response;
}

const std::unordered_map<std::string, Handler> handlers {
    {"set", executeSet},
    {"get", executeGet},
    {"del", executeDel},
    {"ping", executePing}
};

std::string CommandProcessor::execute(const Command& command) {
    std::string cmd_name_lower = command.name;

    std::transform(
        cmd_name_lower.begin(),
        cmd_name_lower.end(), 
        cmd_name_lower.begin(),
        [](unsigned char c) {
            return static_cast<char>(std::tolower(c));
        }
    );
    
    auto it = handlers.find(cmd_name_lower);
    if (it != handlers.end())
        return it->second(command);
    else if(command.args.empty())
        return concatenate(resp::ERROR_UNKNOWN_COMMAND, command.name, "'", resp::CRLF);
    else
        return concatenate(
            resp::ERROR_UNKNOWN_COMMAND,
            command.name,
            resp::ERROR_UNKNOWN_COMMAND_MULTIPLE_ARGUMENTS,
            command.args[0],
            "'",
            resp::CRLF);
}