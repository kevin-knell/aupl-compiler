#pragma once

#include "native.hpp"

namespace auplib {

struct Color {
	float r, g, b, a;
	Color();
	Color(float r, float g, float b, float a = 1.0f) : r(r), g(g), b(b), a(a) {}
};

MARK_TRIVIAL(Color)

}