#include "RESPParser.h"
#include <vector>
#include <string>



class ValueRESP {
public:
    // specific constructors
    ValueRESP(int64_t num) {}
    ValueRESP(std::string s) {}
    Type getType() const {}
private:
    enum class Type {
        SimpleString,
        Integer,
        Array,
        BulkString,
        Error
    };

    Type t_ = Type::SimpleString;
    std::string s_ = "";
    int64_t num_ = 0;
};


ValueRESP::Type getType() const {
        return type_;
    }

ValueRESP::ValueRESP(int64_t num)
        : t_{Type::Integer}
        , num_{num} {}

ValueRESP::ValueRESP(std::string s)
        : t_{Type::SimpleString}
        , s_{s} {}


ValueRESP parseRESP(const std::string& buffer) {
    std::vector<std::string> args;
    size_t pos = 0;

    char c = buffer[pos];
    
    switch (c) {
        case ':': return parseInt(buffer, pos);
        case '*': return parseArray(buffer, pos);
        case '+': return parseSimpleString(buffer, pos);
	case '$': return parseBulkString(buffer, pos);
    }
}

ValueRESP parseInt(const std::string& buffer, size_t& pos) {
    ++pos;
    std::string s = "";
    while (buffer[pos] != '\r') {
        s.push_back(buffer[pos];
        ++pos;
    }
    
    pos += 2;
    return ValueRESP v(std::atoi(s));
}


