#pragma once

#include <iostream>

#include "native.hpp"

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

    void set_x(float _x);
    float get_x() const;
    
    void set_y(float _y);
    float get_y() const;

    // swizzle
	SWIZZLE2(DECL_SET2)
	SWIZZLE2(DECL_GET2)

    // vector functions

    float dot(vec2 v) const;

    float length_squared() const {
        return x * x + y * y;
    }

    float length() const;

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
};
static_assert(sizeof(vec2) == 8);

MARK_TRIVIAL(vec2)

}