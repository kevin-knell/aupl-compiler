#include "viewport.hpp"

namespace auplib {

void Viewport::register_to_db(vm::ClassDB &db) {
	const int16_t ID = REGISTER_CLASS(Viewport);

	REGISTER_VARIABLE(ID, Shared<Scene>, scene);
}

}