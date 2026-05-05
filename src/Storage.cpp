#include "Storage.h"

	
void Storage::set(const std::string& key, const std::string& val) {
	store_[key] = val;
}

std::optional<std::string> Storage::get(const std::string& key) const {
	auto it = store_.find(key);

	if (it != store_.end()) {
		return it->second;
	}

	return std::nullopt;
}

bool Storage::del(const std::string& key) {
	return store_.erase(key);
}
