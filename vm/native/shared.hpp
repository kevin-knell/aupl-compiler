#pragma once
#include <cstddef>
#include <utility>
#include "value.hpp"
#include "native.hpp"
#include "class_db.hpp"

namespace vm {
	class ClassDB;
}

struct SharedData {
	size_t ref_count;
};

template<typename T>
class Shared {
public: //private:
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

	explicit Shared(T* ptr) : obj(ptr) {
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

	static Shared<vm::Value> make_raw(size_t size) {
		return Shared<vm::Value>(new vm::Value[size]);
	}
};


#define REGISTER_SHARED(T) \
	do { \
		const int16_t id = REGISTER_CLASS(Shared<T>); \
		if constexpr(std::is_same_v<T, vm::Value>) { \
			REGISTER_STATIC_METHOD(id, Shared<vm::Value>, make_raw, Shared<vm::Value> (*)(size_t size)); \
		} \
	} while(0);


static_assert(sizeof(Shared<int>) == 16);
