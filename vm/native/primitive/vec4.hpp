#pragma once
#include "native.hpp"

namespace auplib {

struct vec4 {
	float x, y, z, w;
};

MARK_TRIVIAL(vec4)

static_assert(sizeof(vec4) == 16);

}