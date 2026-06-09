#include "native.hpp"

#include "vec2.hpp"

namespace auplib {

struct mat3x2 {
	vec2 x, y;
};

MARK_TRIVIAL(mat3x2)

}