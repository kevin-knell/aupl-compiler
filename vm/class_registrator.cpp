#include "class_registrator.hpp"

#include "string.hpp"
#include "shared.hpp"

#ifndef MINIMAL_VM
#include "vec2.hpp"
#include "console.hpp"
#include "list.hpp"
#include "math.hpp"
#include "file.hpp"
#endif

void vm::register_classes(vm::ClassDB& db) {
	// lang-integrated
	String::register_to_db(db);
	register_shared_to_db(db);
	
#ifndef MINIMAL_VM
	// util
	Console::register_to_db(db);
	Math::register_to_db(db);

	// simple types
	vec2::register_to_db(db);

	// object types
	// data structures
	register_list8_to_db(db);
	
	// other
	File::register_to_db(db);
#endif
}