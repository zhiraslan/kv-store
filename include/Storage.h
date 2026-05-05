#include<unordered_map>
#include<string>
#include<optional>

#pragma once


class Storage {
public:
	void set(const std::string& key, const std::string& val);

	std::optional<std::string> get(const std::string& key) const;

	bool del(const std::string& key);

private:
	std::unordered_map<std::string, std::string> store_;
};