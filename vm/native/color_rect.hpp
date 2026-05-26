#pragma once

#include "object.hpp"
#include "vec2.hpp"
#include "color.hpp"

namespace auplib
{

struct ColorRect : public Object {
	vec2 position;
	vec2 size;
	Color color;

	ColorRect(vec2 position, vec2 size, Color color) : position(position), size(size), color(color) {}
};

} // namespace auplib