#pragma once

#include <cassert>
#include "Storage.h"


enum class Status {
	OK,
	NOT_FOUND,
	INVALID_KEY
};


template<typename T>
class Result {
public:
	// ����������� ��� ��
	Result(const T& val)
		: val_{ val }, status_{ Status::OK }, isOk_{true} {}

	// ����������� ��� ������
	Result(Status status)
		: status_{status}, isOk_{ false} {}

	bool ok() const {
		return isOk_;
	}

	const T& val() const{
		assert(ok());
		
		//if (!ok()) {
		//	std::terminate();
		//}
		return val_;
	}

	Status status() const {
		return status_;
	}

	bool operator==(const Result<T>& r) const {
		if (isOk_ != r.isOk_) {
			return false;
		}

		if (!isOk_) {
			return status_ == r.status_;
		}

		return val_ == r.val_;
	}
private:
	T val_{};
	Status status_{ Status::OK };
	bool isOk_{ false };
};


class Service
{
public:
	explicit Service(Storage& storage);

	Status set(const std::string& key, const std::string& val);

	Result<std::string> get(const std::string& key) const;

	Status del(const std::string& key);
private:
	static const size_t MAX_KEY_SIZE = 256;

	bool isKeyValid(const std::string& key) const;

	// Storage ������ ���� ������ Service, ����� ����� ������� ������.
	// ���� �������� ��������, ����� ������������ std::shared_ptr<Storage> ��� ����������� ��������.
	Storage& storage_; 
};