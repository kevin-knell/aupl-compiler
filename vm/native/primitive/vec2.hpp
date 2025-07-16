#pragma once
#include "primitive.hpp"

namespace vm {
    class ClassDB;
} // namespace vm


class vec2 : public Primitive {
public:
    static void register_to_db(vm::ClassDB& db);

    static const vec2 ZERO;
    static const vec2 ONE;
    static const vec2 EX;
    static const vec2 EY;

    double x, y;


    vec2() = default;

    vec2(double x, double y) : x(x), y(y) {}

    void set_x(double _x);
    double get_x() const;
    
    void set_y(double _y);
    double get_y() const;

    // swizzle
    void set_yx(vec2 v);
    void set_xx(vec2 v);
    void set_yy(vec2 v);

    vec2 get_yx() const;
    vec2 get_xx() const;
    vec2 get_yy() const;

    // vector functions

    double dot(vec2 v) const;

    double length_squared() const {
        return x * x + y * y;
    }

    double length() const;

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

    vec2 operator*(const double& s) const {
        return vec2(x * s, y * s);
    }

    vec2 operator/(const double& s) const {
        return vec2(x / s, y / s);
    }
};
static_assert(sizeof(vec2) == 16);

MARK_PRIMITIVE(vec2)