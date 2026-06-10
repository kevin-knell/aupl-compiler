#include "color.hpp"
#include "class_db.hpp"

namespace auplib {

void Color::register_to_db(vm::ClassDB &db) {
        const int16_t id = REGISTER_CLASS(Color);
		
		REGISTER_CONSTRUCTOR(id, Color());
		REGISTER_CONSTRUCTOR(id, Color(float r, float g, float b, float a));

		REGISTER_SETGET(id, Color, float, r);
		REGISTER_SETGET(id, Color, float, g);
		REGISTER_SETGET(id, Color, float, b);
		REGISTER_SETGET(id, Color, float, a);

}

}