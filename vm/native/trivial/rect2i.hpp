#pragma once

#include "vec2i.hpp"

namespace auplib {

struct Rect2i {
	vec2i start;
	vec2i size;

	Rect2i() = default;
	Rect2i(vec2i start, vec2i size) : start(start), size(size) {}
	Rect2i(int32_t x, int32_t y, int32_t w, int32_t h) : start{x, y}, size{w, h} {}

	vec2i get_end() const {
		return start + size;
	}

	float area() const {
		return size.x * size.y;
	}
};

MARK_TRIVIAL(Rect2i);

} // namespace auplib
