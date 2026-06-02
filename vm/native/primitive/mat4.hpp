#pragma once
#include "native.hpp"
#include "vec4.hpp"

namespace auplib {

struct mat4 {
	union {
		float cells[16];
		vec4 rows[4];

		struct {
			vec4 x;
			vec4 y;
			vec4 z;
			vec4 w;
		};
	};
};

MARK_TRIVIAL(mat4)
static_assert(sizeof(mat4) == 64);

}