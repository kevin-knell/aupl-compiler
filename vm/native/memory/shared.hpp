#pragma once
#include <cstddef>
#include <utility>
#include "value.hpp"
#include "native.hpp"
#include "class_db.hpp"
#include "ref_counted.hpp"
#include <type_traits>

namespace vm {
	class ClassDB;
}

namespace auplib {

template<typename T>
class Shared {
public: //private:
	T* obj;

	static constexpr bool is_ref_counted = std::is_base_of_v<RefCounted, T>;

	void retain() noexcept {
		if constexpr(is_ref_counted) {
			++obj->ref_count;
		}
	}

	void release() noexcept {
		if constexpr(is_ref_counted) {
			if (--obj->ref_count == 0) {
				delete obj;
			}
		}
	}

public:
	constexpr Shared() noexcept : obj(nullptr) {}
	explicit Shared(T* ptr) noexcept : obj(ptr) {
		retain();
	}

	// copy
	constexpr Shared(const Shared& other) noexcept
			: obj(other.obj) {
		retain();
	}

	Shared& operator=(const Shared& other) noexcept {
		if (this != &other) {
			obj = other.obj;
			retain();
		}
		return *this;
	}

	// move
	Shared(Shared&& other) noexcept
			: obj(other.obj) {
		other.obj = nullptr;
	}

	Shared& operator=(Shared&& other) noexcept {
		if (this != &other) {
			obj = other.obj;
			other.obj = nullptr;
		}
		return *this;
	}

	// dtor
	~Shared() {
		release();
	}

	// convert
	template<typename U>
    Shared(const Shared<U>& other)
        	requires std::is_convertible_v<U*, T*> {
		obj = other.obj;
		retain();
    }

	// get
	T* get() const noexcept { return obj; }
	T& operator*() const noexcept { return *obj; }
	T* operator->() const noexcept { return obj; }
	explicit operator bool() const noexcept { return !!obj; }

	// make
	template<typename... Args>
	static Shared make(Args&&... args) {
		if constexpr(is_ref_counted) {
			return Shared(new T(RefCounted::RcKey(), std::forward<Args>(args)...));
		} else {
			return Shared(new T(std::forward<Args>(args)...));
		}
	}
};

static_assert(sizeof(Shared<int>) == 8);
static_assert(sizeof(Shared<RefCounted>) == 8);

}

#define REGISTER_SHARED(T) \
	do { \
		const int16_t id = REGISTER_CLASS(Shared<T>); \
		(void)id; \
		if constexpr(std::is_same_v<T, vm::Value>) { \
		} \
	} while(0);
