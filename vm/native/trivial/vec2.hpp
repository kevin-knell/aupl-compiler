#pragma once

#include <iostream>

#include "native.hpp"
#include "vec2i.hpp"
#include "macros.hpp"
#include "math.hpp"

// Generate all 2-component swizzles for x,y,z
// Declaration helpers

#define SWIZZLE2(M) \
    M(x, x) M(x, y) \
    M(y, x) M(y, y)

#define DECL_SET2(a, b) void set_##a##b(vec2 v) { a = v.x; b = v.y; }
#define DECL_GET2(a, b) vec2 get_##a##b() const { return vec2(a, b); }

namespace vm {
    class ClassDB;
} // namespace vm

namespace auplib {

struct vec2 {
    static void register_to_db(vm::ClassDB& db);

    static const vec2 ZERO;
    static const vec2 ONE;
    static const vec2 EX;
    static const vec2 EY;

    float x, y;

    constexpr vec2() = default;
    constexpr vec2(float v) : x(v), y(v) {}
    constexpr vec2(float x, float y) : x(x), y(y) {}
	constexpr vec2(vec2i v) : x(static_cast<float>(v.x)), y(static_cast<float>(v.y)) {}

	SETGET(x)
	SETGET(y)

    // swizzle
	SWIZZLE2(DECL_SET2)
	SWIZZLE2(DECL_GET2)

    // vector functions

    float dot(vec2 v) const;

    float length_squared() const {
        return x * x + y * y;
    }

    float length() const;

	vec2 normalized() const {
		return vec2(*this) / length();
	}

	vec2 abs() const {
		return vec2(Math::abs(x), Math::abs(y));
	}

	bool linearly_independent(const vec2& other) const {
		return *this != other.abs();
	}

	float angle() {
		double a = Math::atan2(-y, x);
		if (a < 0.0f) a += Math::TAU;
		return static_cast<float>(a);
	}

    // operators
    vec2 operator+(const vec2& other) const {
        return vec2(x + other.x, y + other.y);
    }

    vec2 operator-(const vec2& other) const {
        return vec2(x - other.x, y - other.y);
    }

    vec2 operator-() const {
        return vec2(-x, -y);
    }

    vec2 operator*(const float& s) const {
        return vec2(x * s, y * s);
    }

    vec2 operator/(const float& s) const {
        return vec2(x / s, y / s);
    }

	bool operator!=(const vec2& other) const {
		return x != other.x || y != other.y;
	}

	operator String() const;
};
static_assert(sizeof(vec2) == 8);

MARK_TRIVIAL(vec2)

}