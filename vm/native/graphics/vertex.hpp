#pragma once
#include "vec2.hpp"
#include "vec3.hpp"

namespace auplib
{

struct Vertex {
	vec3 position;
	vec3 color;
	vec2 uv;

	Vertex() = default;
	Vertex(vec3 p, vec3 c, vec2 uv) : position(p), color(c), uv(uv) {}
};

} // namespace auplib
