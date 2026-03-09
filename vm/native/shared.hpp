#pragma once
#include <cstddef>
#include <utility>
#include "value.hpp"
#include "native.hpp"
#include "class_db.hpp"

namespace vm
{
	class ClassDB;
}

struct SharedData {
	size_t ref_count;
};

template<typename T>
class Shared {
private:
	T* obj;
	SharedData* data;

	void retain() noexcept {
		if (data) {
			++data->ref_count;
		}
	}

	void release() noexcept {
		if (data) {
			if (--data->ref_count == 0) {
				delete obj;
				delete data;
			}
			obj = nullptr;
			data = nullptr;
		}
	}

public:
	Shared() noexcept
		: obj(nullptr), data(nullptr) {}

	explicit Shared(T* ptr)
		: obj(ptr) {
		if (ptr) {
			data = new SharedData{1};
		} else {
			data = nullptr;
		}
	}

	Shared(const Shared& other) noexcept
		: obj(other.obj), data(other.data) {
		retain();
	}

	Shared(Shared&& other) noexcept
		: obj(other.obj), data(other.data) {
		other.obj = nullptr;
		other.data = nullptr;
	}

	Shared& operator=(const Shared& other) noexcept {
		if (this != &other) {
			release();
			obj = other.obj;
			data = other.data;
			retain();
		}
		return *this;
	}

	Shared& operator=(Shared&& other) noexcept {
		if (this != &other) {
			release();
			obj = other.obj;
			data = other.data;
			other.obj = nullptr;
			other.data = nullptr;
		}
		return *this;
	}

	~Shared() {
		release();
	}

	T* get() const noexcept { return obj; }
	T& operator*() const noexcept { return *obj; }
	T* operator->() const noexcept { return obj; }

	explicit operator bool() const noexcept { return obj != nullptr; }

	size_t use_count() const noexcept {
		return data ? data->ref_count : 0;
	}

	void reset() noexcept {
		release();
	}

	template<typename... Args>
	static Shared make(Args&&... args) {
		return Shared(new T(std::forward<Args>(args)...));
	}
};

static_assert(sizeof(Shared<int>) == 16);

using Shared8 = Shared<int64_t>;

void register_shared_to_db(vm::ClassDB db) {

}