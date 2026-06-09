#include "vec3.hpp"
#include "math.hpp"

namespace auplib {

float vec3::length() const { return Math::sqrt(length_squared()); }

} // namespace auplib
