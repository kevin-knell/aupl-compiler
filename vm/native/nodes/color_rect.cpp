#include "color_rect.hpp"
#include "class_db.hpp"

namespace auplib {

void ColorRect::register_to_db(vm::ClassDB &db) {
	const int16_t ID = REGISTER_CLASS(ColorRect);
	
	REGISTER_CONSTRUCTOR(ID, ColorRect(vec2 position, vec2 size, Color color));
	
	REGISTER_VARIABLE(ID, vec2, position);
	REGISTER_VARIABLE(ID, vec2, size);
	REGISTER_VARIABLE(ID, Color, color);

}

}