#include "ValueRESP.h"


ValueRESP::Type getType() const {
        return type_;
    }

ValueRESP::ValueRESP(int64_t num)
        : type_{Type::Integer}
        , num_{num} {}

ValueRESP::ValueRESP(std::string s, bool isBulk)
        : type_{isBulk ? Type::BulkString : Type::SimpleString}
        , s_{s} {}
