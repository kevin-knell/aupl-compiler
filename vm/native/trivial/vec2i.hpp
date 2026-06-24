#pragma once
#include <cstdint>
#include "native.hpp"
#include "macros.hpp"

#define SWIZZLE2(M) \
    M(x, x) M(x, y) \
    M(y, x) M(y, y)

#define DECL_SET2I(a, b) void set_##a##b(vec2i v) { a = v.x; b = v.y; }
#define DECL_GET2I(a, b) vec2i get_##a##b() const { return vec2i(a, b); }

namespace vm {
    class ClassDB;
} // namespace vm

namespace auplib {

struct vec2i {
    static void register_to_db(vm::ClassDB& db);

    static const vec2i ZERO;
    static const vec2i ONE;
    static const vec2i EX;
    static const vec2i EY;

    int32_t x, y;

    vec2i() = default;
    vec2i(int32_t x, int32_t y) : x(x), y(y) {}

	SETGET(x)
	SETGET(y)

    // swizzle
	SWIZZLE2(DECL_SET2I)
	SWIZZLE2(DECL_GET2I)

	int32_t area() const;

	// operators
	vec2i operator+(const vec2i& other) const {
		return vec2i(x + other.x, y + other.y);
	}

	vec2i operator-(const vec2i& other) const {
		return vec2i(x - other.x, y - other.y);
	}

	vec2i operator-() const {
		return vec2i(-x, -y);
	}

	vec2i operator*(const int32_t& s) const {
		return vec2i(x * s, y * s);
	}

	bool operator!=(const vec2i& other) const {
		return x != other.x || y != other.y;
	}

	operator String() const;
};

MARK_TRIVIAL(vec2i)

}