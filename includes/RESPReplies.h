#include <string_view>


namespace resp {
    inline constexpr std::string_view CRLF = "\r\n";
    inline constexpr std::string_view OK = "+OK\r\n";
    inline constexpr std::string_view PONG = "+PONG\r\n";
    inline constexpr std::string_view ERROR_PROTOCOL = "-ERR Protocol error\r\n";
    inline constexpr std::string_view ERROR_EMPTY_COMMAND = "-ERR empty command\r\n";
    inline constexpr std::string_view ERROR_UNKNOWN_COMMAND = "-ERR unknown command '";
    inline constexpr std::string_view ERROR_UNKNOWN_COMMAND_MULTIPLE_ARGUMENTS = "', with args beginning with: '";
    inline constexpr std::string_view ERROR_ARGUMENTS_NUMBER = "-ERR wrong number of arguments for '";    
}