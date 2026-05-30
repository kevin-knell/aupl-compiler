#include "class_registrator.hpp"

#include "string.hpp"
#include "shared.hpp"

#ifndef MINIMAL_VM
// utils
#include "console.hpp"
#include "math.hpp"
#include "time.hpp"

// trivials
#include "vec2.hpp"
#include "color.hpp"

// objects
#include "list.hpp"
#include "file.hpp"
#include "window.hpp"
#include "viewport.hpp"
#include "scene.hpp"
#include "renderer.hpp"

// nodes
#include "node.hpp"
#include "color_rect.hpp"
#endif

void register_shared_to_db(vm::ClassDB& db) {
	using auplib::Shared;
	CREATE_GENERICS(REGISTER_SHARED)
}

void vm::register_classes(vm::ClassDB& db) {
	using namespace auplib;
	
	// lang-integrated
	String::register_to_db(db);
	register_shared_to_db(db);
	
#ifndef MINIMAL_VM
	// util
	Console::register_to_db(db);
	Math::register_to_db(db);
	Time::register_to_db(db);

	// simple types
	vec2::register_to_db(db);
	vec2i::register_to_db(db);
	Color::register_to_db(db);

	// object types
	// data structures
	//register_list8_to_db(db);
	
	File::register_to_db(db);
	Window::register_to_db(db);
	Viewport::register_to_db(db);
	Scene::register_to_db(db);
	Renderer::register_to_db(db);

	// nodes
	Node::register_to_db(db);
	ColorRect::register_to_db(db);
#endif
}