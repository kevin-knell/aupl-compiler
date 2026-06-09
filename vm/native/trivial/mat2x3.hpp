#include "native.hpp"

#include "vec3.hpp"

namespace auplib {

struct mat2x3 {
	vec3 x, y;
};

MARK_TRIVIAL(mat2x3)

}