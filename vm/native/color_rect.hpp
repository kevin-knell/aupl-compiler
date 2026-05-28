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

	static void register_to_db(vm::ClassDB& db);

	ColorRect(vec2 position, vec2 size, Color color) : position(position), size(size), color(color) {}
};

} // namespace auplib