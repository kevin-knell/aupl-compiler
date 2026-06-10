#pragma once
#include <cstdint>
#include "native.hpp"

namespace vm {
    class ClassDB;
} // namespace vm

namespace auplib {

struct vec2i {
    static void register_to_db(vm::ClassDB& db);

    static const vec2i ZERO;
    static const vec2i ONE;
    static const vec2i EX;
    static const vec2i EY;

    int32_t x, y;

    vec2i() = default;
    vec2i(int32_t x, int32_t y) : x(x), y(y) {}
	
	int32_t area() const;

	operator String() const;
};

MARK_TRIVIAL(vec2i)

}