#pragma once
#include "native.hpp"
#include "vec4.hpp"

namespace auplib {

struct mat4 {
	vec4 x;
	vec4 y;
	vec4 z;
	vec4 w;

	constexpr mat4() = default;
	constexpr mat4(vec4 x, vec4 y, vec4 z, vec4 w) : x(x), y(y), z(z), w(w) {}
};

MARK_TRIVIAL(mat4)
static_assert(sizeof(mat4) == sizeof(float) * 16);

}