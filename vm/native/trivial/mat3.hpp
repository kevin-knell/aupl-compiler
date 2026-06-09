#pragma once
#include "native.hpp"
#include "vec3.hpp"

namespace auplib {

struct mat3 {
	vec3 x;
	vec3 y;
	vec3 z;
};

MARK_TRIVIAL(mat3)
static_assert(sizeof(mat3) == sizeof(float) * 9);

}