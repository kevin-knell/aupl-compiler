#pragma once
#include "native.hpp"

namespace auplib {

struct vec3 {
	float x, y, z;
};

MARK_TRIVIAL(vec3)

static_assert(sizeof(vec3) == 12);

}