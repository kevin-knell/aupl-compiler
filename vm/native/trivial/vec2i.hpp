#pragma once
#include <cstdint>
#include "native.hpp"

#define SWIZZLE2(M) \
    M(x, x) M(x, y) \
    M(y, x) M(y, y)

#define DECL_SET2I(a, b) void set_##a##b(vec2i v) { a = v.x; b = v.y; }
#define DECL_GET2I(a, b) vec2i get_##a##b() const { return vec2i(a, b); }

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

    void set_x(int32_t _x) { x = _x; }
    int32_t get_x() const { return x; }
    
    void set_y(int32_t _y) { y = _y; }
    int32_t get_y() const { return y; }

    // swizzle
	SWIZZLE2(DECL_SET2I)
	SWIZZLE2(DECL_GET2I)

	int32_t area() const;

	operator String() const;
};

MARK_TRIVIAL(vec2i)

}