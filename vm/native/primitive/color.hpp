#pragma once

#include "native.hpp"

namespace vm {
	class ClassDB;
}

namespace auplib {

struct Color {
	float r, g, b, a;

	static void register_to_db(vm::ClassDB& db);

	Color() = default;
	Color(float r, float g, float b, float a = 1.0f) : r(r), g(g), b(b), a(a) {}
};

MARK_TRIVIAL(Color)

}