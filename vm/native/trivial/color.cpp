#include "color.hpp"
#include "class_db.hpp"

namespace auplib {

void Color::register_to_db(vm::ClassDB &db) {
        const int16_t id = REGISTER_CLASS(Color);
		
		REGISTER_CONSTRUCTOR(id, Color());
		REGISTER_CONSTRUCTOR(id, Color(float r, float g, float b, float a));

		REGISTER_VARIABLE(id, float, r);
		REGISTER_VARIABLE(id, float, g);
		REGISTER_VARIABLE(id, float, b);
		REGISTER_VARIABLE(id, float, a);

}

}