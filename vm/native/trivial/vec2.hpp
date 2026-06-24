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

	COPY_DEFAULT(vec2)
	MOVE_DEFAULT(vec2)

	~vec2() = default;

	SETGET(x)
	SETGET(y)

    // swizzle
	SWIZZLE2(DECL_SET2)
	SWIZZLE2(DECL_GET2)

    // vector functions
	vec2 abs() const;

	float angle() const;
	float angle_to(vec2 other) const;
	float angle_to_point(vec2 p) const;

	float aspect() const;

	vec2 bounce(vec2 norm) const;

	float cross(vec2 other) const;

    float dot(vec2 v) const;

	float length() const;

    float length_squared() const;

	bool linearly_independent(const vec2& other) const;

	vec2 normalized() const;

	// rounding
	vec2 ceil() const;
	vec2 floor() const;
	vec2 round() const;

	// min / max
	vec2 clamp(vec2 min, vec2 max) const;
	vec2 clamp(float min, float max) const;

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