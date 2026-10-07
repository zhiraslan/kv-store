#include <string>
#include <unordered_map>
#include <optional>


template <class T>
class Storage {
public:
    bool set(const std::string& key, const T& value) {
        return data_.insert_or_assign(key, value).second;
    }

    std::optional<T> get(const std::string& key) const {
        auto it = data_.find(key);
        
        if (it != data_.end())
            return it->second;

        return std::nullopt;
    }

    bool del(const std::string& key) {
        // when erasing by key  — returns the number of elements removed
        return data_.erase(key);
    };    
private:
    std::unordered_map<std::string, T> data_;
};