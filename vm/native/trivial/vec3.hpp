#pragma once
#include "native.hpp"

#include "vec2.hpp"

// Generate all 3-component swizzles for x,y,z
#define SWIZZLE2(M) \
    M(x, x) M(x, y) \
    M(y, x) M(y, y)

#define SWIZZLE3(M) \
    M(x, x, x) M(x, x, y) M(x, x, z) M(x, y, x) M(x, y, y) M(x, y, z) M(x, z, x) M(x, z, y) M(x, z, z) \
    M(y, x, x) M(y, x, y) M(y, x, z) M(y, y, x) M(y, y, y) M(y, y, z) M(y, z, x) M(y, z, y) M(y, z, z) \
    M(z, x, x) M(z, x, y) M(z, x, z) M(z, y, x) M(z, y, y) M(z, y, z) M(z, z, x) M(z, z, y) M(z, z, z)

#define DECL_SET3(a, b, c) void set_##a##b##c(vec3 v) { a = v.x; b = v.y; c = v.z; }
#define DECL_GET3(a, b, c) vec3 get_##a##b##c() const { return vec3(a, b, c); }

namespace auplib {

struct vec3 {
	float x, y, z;

    constexpr vec3() = default;
    constexpr vec3(float x, float y, float z) : x(x), y(y), z(z) {}
    constexpr vec3(vec2 v) : x(v.x), y(v.y), z(0.0f) {}

	SETGET(x)
	SETGET(y)
	SETGET(z)

    // swizzle
	SWIZZLE3(DECL_SET3)
	SWIZZLE3(DECL_GET3)

	SWIZZLE2(DECL_SET2)
	SWIZZLE2(DECL_GET2)

    // vector functions

    float dot(vec3 v) const { return x * v.x + y * v.y + z * v.z; }

    float length_squared() const {
        return x * x + y * y + z * z;
    }

    float length() const;

    // operators
    vec3 operator+(const vec3& other) const {
        return vec3(x + other.x, y + other.y, z + other.z);
    }

    vec3 operator-(const vec3& other) const {
        return vec3(x - other.x, y - other.y, z - other.z);
    }

    vec3 operator-() const {
        return vec3(-x, -y, -z);
    }

    vec3 operator*(const float& s) const {
        return vec3(x * s, y * s, z * s);
    }

    vec3 operator/(const float& s) const {
        return vec3(x / s, y / s, z / s);
    }
};

MARK_TRIVIAL(vec3)
static_assert(sizeof(vec3) == sizeof(float) * 3);

}