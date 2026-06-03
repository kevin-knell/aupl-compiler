#include "class_registrator.hpp"

#include "object.hpp"
#include "string.hpp"
#include "shared.hpp"
#include "list.hpp"

#ifndef MINIMAL_VM
// utils
#include "console.hpp"
#include "math.hpp"
#include "time.hpp"
#include "input.hpp"

// trivials
#include "vec2.hpp"
#include "color.hpp"

// objects
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

void register_list_to_db(vm::ClassDB& db) {
	using namespace auplib;
	using T = vm::Value;
	const int16_t ID = REGISTER_CLASS(List<T>);

	REGISTER_CONSTRUCTOR(ID, List<T>());
	
	REGISTER_METHOD(ID, List<T>, push, void (List<T>::*)(T value));
	REGISTER_METHOD(ID, List<T>, pop, T (List<T>::*)());
	REGISTER_METHOD(ID, List<T>, get, T (List<T>::*)(size_t idx));
	REGISTER_METHOD(ID, List<T>, set, void (List<T>::*)(size_t idx, T value));
	REGISTER_METHOD(ID, List<T>, size, size_t (List<T>::*)());
	REGISTER_METHOD(ID, List<T>, empty, bool (List<T>::*)());
	REGISTER_METHOD(ID, List<T>, clear, void (List<T>::*)());
	REGISTER_METHOD(ID, List<T>, operator[], T& (List<T>::*)(size_t idx));
}

void vm::register_classes(vm::ClassDB& db) {
	using namespace auplib;
	
	// lang-integrated
	String::register_to_db(db);
	register_shared_to_db(db);
	Object::register_to_db(db);
	register_list_to_db(db);
	
#ifndef MINIMAL_VM
	// util
	Console::register_to_db(db);
	Math::register_to_db(db);
	Time::register_to_db(db);
	Input::register_to_db(db);

	// simple types
	vec2::register_to_db(db);
	vec2i::register_to_db(db);
	Color::register_to_db(db);

	// object types
	// data structures
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