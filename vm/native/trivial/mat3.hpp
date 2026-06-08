#pragma once
#include "native.hpp"
#include "vec3.hpp"

namespace auplib {

class mat3 {
	union {
		float cells[9];
		vec3 rows[3];

		struct {
			vec3 x;
			vec3 y;
			vec3 z;
		};
	};
};

MARK_TRIVIAL(mat3)

}