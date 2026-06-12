#include "curve_2d.hpp"

namespace auplib {

void Curve2D::register_to_db(vm::ClassDB &db) {
	const int16_t ID = REGISTER_OBJECT_CLASS(Curve2D, CanvasItem);
	
	REGISTER_CONSTRUCTOR(ID, Curve2D());

	REGISTER_METHOD(ID, Curve2D, add_point, void (Curve2D::*)(vec2 p));
}

}