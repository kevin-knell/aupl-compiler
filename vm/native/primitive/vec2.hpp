#pragma once

#include <iostream>

#include "native.hpp"

namespace vm {
    class ClassDB;
} // namespace vm

namespace auplib {

struct vec2 {
    static void register_to_db(vm::ClassDB& db);

    static const vec2 ZERO;
    static const vec2 ONE;
    static const vec2 EX;
    static const vec2 EY;

    float x, y;


    vec2() = default;

    vec2(float x, float y) : x(x), y(y) {}

    void set_x(float _x);
    float get_x() const;
    
    void set_y(float _y);
    float get_y() const;

    // swizzle
    void set_yx(vec2 v);
    void set_xx(vec2 v);
    void set_yy(vec2 v);

    vec2 get_yx() const;
    vec2 get_xx() const;
    vec2 get_yy() const;

    // vector functions

    float dot(vec2 v) const;

    float length_squared() const {
        return x * x + y * y;
    }

    float length() const;

    // operators
    vec2 operator+(const vec2& other) const {
        return vec2(x + other.x, y + other.y);
    }

    vec2 operator-(const vec2& other) const {
        return vec2(x - other.x, y - other.y);
    }

    vec2 operator-() const {
        return vec2(-x, -y);
    }

    vec2 operator*(const float& s) const {
        return vec2(x * s, y * s);
    }

    vec2 operator/(const float& s) const {
        return vec2(x / s, y / s);
    }
};
static_assert(sizeof(vec2) == 8);

MARK_TRIVIAL(vec2)

}