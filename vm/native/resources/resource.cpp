#include "resource.hpp"

#include "class_db.hpp"

namespace auplib {

void Resource::register_to_db(vm::ClassDB &db) {
	const uint16_t ID = REGISTER_OBJECT_CLASS(Resource, RefCounted);
	(void)ID;
}

}