#pragma once

#include <string>
#include <vector>
#include <cstddef>
#include "ParseContent.h"


namespace resp_limits {
    inline constexpr std::size_t MAX_PAYLOAD_NUMBER = 1024;
    inline constexpr std::size_t MAX_PAYLOAD_SIZE = 1024 * 1024;
    inline constexpr std::size_t MAX_COMMAND_BYTES = 4 * 1024 * 1024;
}


enum class PARSE_STATUS {    
    COMPLETE,
    INCOMPLETE,
    ERROR
};

struct ParseResult {
    PARSE_STATUS status;
    std::size_t bytes_parsed = 0;
};

class RESPParser {
public:
    ParseResult parseRESP(const std::string& input, ParseContent& pc);
};

