#pragma once

#include "mat4.hpp"
#include "mat3.hpp"

namespace auplib {

struct ObjectUBO {
	vec4 size = vec4(1.0, 1.0, 1.0, 1.0);

	mat4 model{
		vec4::EX,
		vec4::EY,
		vec4::EZ,
		vec4::EW
	};
	mat4 model_view;
	mat4 model_view_projection;

	mat3 normal_matrix;
};

static_assert(offsetof(ObjectUBO, model) == sizeof(ObjectUBO::size));
	
} // namespace auplib
