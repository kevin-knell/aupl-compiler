#pragma once

#include "vec2.hpp"

namespace auplib {

struct Rect2 {
	vec2 start;
	vec2 size;

	Rect2() = default;
	Rect2(vec2 start, vec2 size) : start(start), size(size) {}
	Rect2(float x, float y, float w, float h) : start{x, y}, size{w, h} {}

	vec2 get_end() const {
		return start + size;
	}
};

MARK_TRIVIAL(Rect2);

} // namespace auplib
