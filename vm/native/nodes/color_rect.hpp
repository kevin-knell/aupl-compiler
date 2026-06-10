#pragma once

#include "canvas_item.hpp"
#include "vec2.hpp"
#include "color.hpp"

namespace auplib {

struct ColorRect : public CanvasItem {
	vec2 position;
	vec2 size;
	Color color;

	static void register_to_db(vm::ClassDB& db);

	ColorRect(vec2 position, vec2 size, Color color) : CanvasItem(), position(position), size(size), color(color) {}

	ColorRect(const ColorRect&) = delete;
	ColorRect& operator=(ColorRect&) = delete;
};

} // namespace auplib