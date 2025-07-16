#include "vec2.hpp"

#include <cmath>

#include "native.hpp"
#include "class_db.hpp"
#include "complex.hpp"

const vec2 vec2::ZERO(0, 0);
const vec2 vec2::ONE(0, 0);
const vec2 vec2::EX(1, 0);
const vec2 vec2::EY(0, 1);

void vec2::set_x(double _x) { x = _x; }
double vec2::get_x() const { return x; }

void vec2::set_y(double _y) { y = _y; }
double vec2::get_y() const { return y; }

void vec2::set_xx(vec2 v) { x = v.x; x = v.y; }
void vec2::set_yx(vec2 v) { y = v.x; x = v.y; }
void vec2::set_yy(vec2 v) { y = v.x; y = v.y; }

vec2 vec2::get_xx() const { return vec2(x, x); }
vec2 vec2::get_yx() const { return vec2(y, x); }
vec2 vec2::get_yy() const { return vec2(y, y); }

double vec2::dot(vec2 v) const
{
    return x * v.x + y * v.y;
}

double vec2::length() const
{
    return std::sqrt(length_squared());
}

void vec2::register_to_db(vm::ClassDB& db) {
        constexpr int id = 0;

        REGISTER_CLASS(id, vec2);
        REGISTER_METHOD(id, vec2, set_x);
        REGISTER_METHOD(id, vec2, get_x);
        REGISTER_METHOD(id, vec2, set_y);
        REGISTER_METHOD(id, vec2, get_y);

        REGISTER_METHOD(id, vec2, set_xx);
        REGISTER_METHOD(id, vec2, set_yx);
        REGISTER_METHOD(id, vec2, set_yy);

        REGISTER_METHOD(id, vec2, get_xx);
        REGISTER_METHOD(id, vec2, get_yx);
        REGISTER_METHOD(id, vec2, get_yy);

        REGISTER_METHOD(id, vec2, dot);
        REGISTER_METHOD(id, vec2, length_squared);
        REGISTER_METHOD(id, vec2, length);

        REGISTER_METHOD(id, vec2, operator+);
        REGISTER_METHOD(id, vec2, operator-, vec2 (vec2::*)() const);
        REGISTER_METHOD(id, vec2, operator-, vec2 (vec2::*)(const vec2&) const);
        REGISTER_METHOD(id, vec2, operator*);
        REGISTER_METHOD(id, vec2, operator/);
}