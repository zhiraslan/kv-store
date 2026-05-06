class ValueRESP {
public:
    // specific constructors
    ValueRESP(int64_t num);
    ValueRESP(std::string s, bool isBulk);
    Type getType() const;
private:
    enum class Type {
        SimpleString,
        Integer,
        Array,
        BulkString,
        Error
    };

    Type type_ = Type::SimpleString;
    std::string s_;
    int64_t num_ = 0;
};
