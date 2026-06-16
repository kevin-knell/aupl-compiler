#include "sprite_2d.hpp"
#include "draw_util.hpp"

namespace auplib {

void Sprite2D::register_to_db(vm::ClassDB &db) {
	const uint16_t ID = REGISTER_OBJECT_CLASS(Sprite2D, CanvasItem);

	REGISTER_CONSTRUCTOR(ID, Sprite2D());
	REGISTER_CONSTRUCTOR(ID, Sprite2D(Shared<Texture> tex));
}

void Sprite2D::draw() const {
	DrawUtil::clear_commands(get_canvas_item_rid());
	DrawUtil::draw_rectangle(
		get_canvas_item_rid(),
		texture->get_rid(),
		Rect2(-size / 2.0, size),
		Rect2(-size / 2.0, size)
	);
}

}