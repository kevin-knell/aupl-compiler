#include "ref_counted.hpp"

#include "class_db.hpp"

namespace auplib {

void RefCounted::register_to_db(vm::ClassDB &db) {
	const uint16_t ID = REGISTER_OBJECT_CLASS(RefCounted, Object);
	(void)ID;
}

}