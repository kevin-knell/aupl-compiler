#pragma once

#include "canvas_item.hpp"
#include "vec2.hpp"
#include "color.hpp"
#include "macros.hpp"

namespace auplib {

struct ColorRect : public CanvasItem {
	Color color;

	static void register_to_db(vm::ClassDB& db);

	ColorRect(vec2 position, vec2 size, Color color) : CanvasItem(), color(color) {
		set_position(position);
		set_size(size);
	}

	COPY_DELETE(ColorRect)
};

} // namespace auplib