#include "Service.h"


Status Service::set(const std::string& key, const std::string& val) {
	if (!isKeyValid(key)) {
		return Status::INVALID_KEY;
	}

	storage_.set(key, val);
	
	return Status::OK;
}


Result<std::string> Service::get(const std::string& key) const {
	if (!isKeyValid(key)) {
		return Status::INVALID_KEY;
	}

	auto val = storage_.get(key);

	if (val) {
		return *val;
	}
	else {
		return Status::NOT_FOUND;
	}
}


Service::Service(Storage& storage)
	: storage_{ storage } {};


Status Service::del(const std::string& key) {
	if (!isKeyValid(key)) {
		return Status::INVALID_KEY;
	}

	if (storage_.del(key)) {
		return Status::OK;
	}
	else {
		return Status::NOT_FOUND;
	}
}

bool Service::isKeyValid(const std::string& key) const {
	return !key.empty() && key.size() <= MAX_KEY_SIZE;
}