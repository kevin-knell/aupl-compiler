#include "object.hpp"

#include <cstdint>

#include "class_db.hpp"

namespace auplib {

void Object::register_to_db(vm::ClassDB &db) {
	const int16_t ID = REGISTER_CLASS(Object);
	(void)ID;
}

}