#pragma once
#include "native.hpp"
#include "vec2.hpp"

namespace auplib {

struct vec4 {
    static const vec4 EX;
    static const vec4 EY;
    static const vec4 EZ;
    static const vec4 EW;

	float x, y, z, w;

	vec4() = default;
	vec4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
	vec4(vec2 v, vec2 w) : x(v.x), y(v.y), z(w.x), w(w.y) {}
};

MARK_TRIVIAL(vec4)

static_assert(sizeof(vec4) == 16);

}