#pragma once
#include <cstdint>
#include "native.hpp"

namespace vm {
    class ClassDB;
} // namespace vm

namespace auplib {

class vec2i {
public:
    static void register_to_db(vm::ClassDB& db);

    static const vec2i ZERO;
    static const vec2i ONE;
    static const vec2i EX;
    static const vec2i EY;

    int64_t x, y;

    vec2i() = default;
    vec2i(int64_t x, int64_t y) : x(x), y(y) {}
};

MARK_TRIVIAL(vec2i)

}