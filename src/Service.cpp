#include "Service.h"


Service* Service::getInstance() {
    static Service service;
    return &service;
}

void Service::set(const std::string& key, const std::string& value) {
    storage_.set(key, value);
}

std::optional<std::string> Service::get(const std::string& key) const {
    return storage_.get(key);
}

std::size_t Service::del(const std::vector<std::string>& keys) {
    std::size_t n = 0;

    for (const std::string& key : keys) {
        if (storage_.del(key)) ++n;
    }

    return n;
}


class Service {
public:
    static Service* getInstance();

    Service(const Service&) = delete;
    Service& operator=(const Service&) = delete;

    void set(const std::string& key, const std::string& value);
    std::optional<std::string> get(const std::string& key) const;
    std::size_t del(const std::vector<std::string>& keys);
private:
    Service() = default;
    Storage<std::string> storage_;
};