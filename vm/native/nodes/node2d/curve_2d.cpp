#include "curve_2d.hpp"

namespace auplib {

void Curve2D::register_to_db(vm::ClassDB &db) {
	const int16_t ID = REGISTER_OBJECT_CLASS(Curve2D, CanvasItem);
	
	REGISTER_CONSTRUCTOR(ID, Curve2D(vec2 a, vec2 b, vec2 c, vec2 d));
}

}