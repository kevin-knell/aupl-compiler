#include "sprite_2d.hpp"

namespace auplib {

void Sprite2D::register_to_db(vm::ClassDB &db) {
	const uint16_t ID = REGISTER_OBJECT_CLASS(Sprite2D, CanvasItem);

	REGISTER_CONSTRUCTOR(ID, Sprite2D());
	REGISTER_CONSTRUCTOR(ID, Sprite2D(Shared<Image> tex));
}

}