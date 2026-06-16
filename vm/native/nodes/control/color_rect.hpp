#pragma once

#include "canvas_item.hpp"
#include "vec2.hpp"
#include "color.hpp"
#include "macros.hpp"
#include "texture_storage.hpp"
#include "draw_util.hpp"
#include "image.hpp"

namespace auplib {

class ColorRect : public CanvasItem {
	OBJECT_CLASS(ColorRect, CanvasItem)
public:
	Color color;
	vec2 size;

	static void register_to_db(vm::ClassDB& db);

	ColorRect(vec2 position, vec2 size, Color color) : CanvasItem(), color(color), size(size) {
		static Shared<Image> image;
		static TextureRID tex;
		
		if (!image) {
			image = Image::create_fill(vec2i(1, 1), Color(1.0));
			tex = TextureStorage::singleton()->create_texture(image.get());
		}
		
		set_position(position);

		DrawUtil::draw_rectangle(
			get_canvas_item_rid(),
			tex,
			Rect2(vec2(), size),
			Rect2(vec2(), size)
		);
	}

	COPY_DELETE(ColorRect)

	SETGET(size)
};

} // namespace auplib