#pragma once

#include <vector>
#include <string>
#include <optional>
#include <cstddef>
#include "Storage.h"


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