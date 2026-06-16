#include "mat2.hpp"

namespace auplib {

mat2 mat2::from_angle(float angle) {
	return mat2(
		{ Math::cos(angle), Math::sin(angle) },
		{ -Math::sin(angle), Math::cos(angle) }
	);
}

}