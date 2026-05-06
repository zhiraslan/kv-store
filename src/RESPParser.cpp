#include "RESPParser.h"
#include <vector>
#include <string>


class RESPParser {
public:
    ValueRESP parse(const std::string& buffer);

private:
    const std::string* buffer();
    size_t pos_;

    ValueRESP parseValue();
    ValueRESP parseInt();
    ValueRESP parseSimpleString();
    ValueRESP parseSimpleString();
};


ValueRESP RESPParser::parseValue() {
    if (pos_ >= buffer.size()) {
	throw stdd:runtime_error("RESP: unexpected end.");
    }

    char c = buffer->at(pos_);

    switch (c) {
        case ':': return parseInt();
        case '+': return parseSimpleString();
	case '$': return parseBulkString();
	case '*': return parseArray();
	default: throw std::runtime_error("unknown RESP type.");
    }
}


ValueRESP::ValueRESP parseSimpleString(const std::string& buffer, size_t& pos) {
    ++pos;
    std::string s;
    while (pos < buffer.size() && buffer[pos] != '\r') {
        s.push_back(buffer[pos]);
        ++pos;
    }

    if (pos+1 >= buffer.size() ||
        buffer[pos] != '\r' ||
        buffer[pos + 1] != '\n') {
        throw std::runtime_error("RESP parse error: invalid simple string format.");
    }

    pos += 2;
    return ValueRESP(s);
}


ValueRESP::ValueRESP parseInt(const std::string& buffer, size_t& pos) {
    ++pos;
    std::string s;
    while (pos < buffer.size() && buffer[pos] != '\r') {
        s.push_back(buffer[pos]);
        ++pos;
    }
    
    if (pos + 1 >= buffer.size() || 
	buffer[pos] != '\r' ||
	buffer[pos + 1] != '\n') {	
	throw std::runtime_error("RESP parse error: invalid integer format.");
    }

    pos += 2;
    return ValueRESP(std::stoll(s), false);
}


ValueRESP::ValueRESP parseBulkString(const std::string& buffer, size_t& pos) {
    ++pos;
    string lenStr;

    while (pos <= buffer.size()нет && buffer[pos] != '\r') {
	lenStr += buffer[pos];
	++pos;
    }

    if (pos + 1 >= buffer.size() ||
	buffer[pos] != '\r' ||
	buffer[pos + 1] != '\n') {
	throw std::runtime_error("RESP parse error: invalid bulk string format (length).");
    }

    pos += 2;
    int64_t strLen = std::stoll(lenStr);
    std::string s;
    s.reserve(strLen);


    for (int i = 0; i < strLen; ++i) {
	if (pos >= buffer.size()) {
	    throw std::runtime_error("RESP parse error: unexpected end of bulk string.");
	}
	s.push_back(buffer[pos++]);
    }

    if (pos + 1 >= buffer.size() ||
        buffer[pos] != '\r' ||
        buffer[pos+1] != '\n') {
        throw std::runtime_error("RESP parse error: invalid bulk string format.");
    }
    pos += 2;
    return ValueRESP(s, true);
}
