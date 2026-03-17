#include "class_registrator.hpp"
#include "vec2.hpp"
#include "string.hpp"
#include "console.hpp"
#include "list.hpp"
#include "math.hpp"
#include "file.hpp"
#include "shared.hpp"

void vm::register_classes(vm::ClassDB& db) {
	// lang-integrated
	String::register_to_db(db);
	register_shared_to_db(db);
	
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
}