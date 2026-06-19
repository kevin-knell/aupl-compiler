#pragma once
#include "native.hpp"
#include "vec2.hpp"
#include <cmath>

#define SET_MAT2_ROW(a) void set_##a(vec2 v) { a = v; }
#define GET_MAT2_ROW(a) vec2 get_##a() const { return a; }

#define SET_MAT_CELL(a, b) void set_##a##b(float v) { a.b = v; }
#define GET_MAT_CELL(a, b) float get_##a##b() const { return a.b; }

namespace auplib {

struct mat2 {
	vec2 x;
	vec2 y;

	static mat2 from_angle(float angle);

	constexpr mat2() = default;
	constexpr mat2(vec2 x, vec2 y) : x(x), y(y) {}

	vec2* rows() { return reinterpret_cast<vec2*>(this); }
	const vec2* rows() const { return reinterpret_cast<const vec2*>(this); }

	float* cells() { return reinterpret_cast<float*>(this); }
	const float* cells() const { return reinterpret_cast<const float*>(this); }

	constexpr size_t size() const { return 4; }

	// set / get row
	SET_MAT2_ROW(x)
	SET_MAT2_ROW(y)

	GET_MAT2_ROW(x)
	GET_MAT2_ROW(y)

	// set / get single value
	SET_MAT_CELL(x, x)
	SET_MAT_CELL(x, y)
	SET_MAT_CELL(y, x)
	SET_MAT_CELL(y, y)

	GET_MAT_CELL(x, x)
	GET_MAT_CELL(x, y)
	GET_MAT_CELL(y, x)
	GET_MAT_CELL(y, y)

	// det(A) = ad - bc
	constexpr float det() const { return x.x * y.y - x.y * y.x; }

	mat2 transposed() const {
		return mat2{
			{ x.x, y.x },
			{ x.y, y.y }
		};
	}

	float angle() const {
		return vec2(y.x, x.x).angle();
	}

	constexpr mat2 operator+(const mat2& other) const { return mat2(x + other.x, y + other.y); }

	constexpr mat2 operator-(const mat2& other) const { return mat2(x - other.x, y - other.y); }

	constexpr mat2 operator*(const mat2& other) const {
		return mat2{
			{ x.x * other.x.x + x.y * other.y.x, x.x * other.x.y + x.y * other.y.y },
			{ y.x * other.x.x + y.y * other.y.x, y.x * other.x.y + y.y * other.y.y }
		};
	}

	constexpr vec2 operator*(const vec2& v) const {
		return vec2(
			x.x * v.x + x.y * v.y,
			y.x * v.x + y.y * v.y
		);
	}

	constexpr mat2 operator*(float s) const {
		return mat2( x * s, y * s );
	}

	constexpr mat2 operator/(float s) const {
		return mat2( x / s, y / s );
	}
};

MARK_TRIVIAL(mat2)
static_assert(sizeof(mat2) == sizeof(float) * 4);

}