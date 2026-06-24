#include "vec2.hpp"

#include <cmath>

#include "native.hpp"
#include "class_db.hpp"
#include "complex.hpp"
#include "math.hpp"
#include "string.hpp"

namespace auplib {

const vec2 vec2::ZERO(0, 0);
const vec2 vec2::ONE(1, 1);
const vec2 vec2::EX(1, 0);
const vec2 vec2::EY(0, 1);

vec2 vec2::abs() const { return vec2(Math::abs(x), Math::abs(y)); }

float vec2::angle() const {
	double a = Math::atan2(-y, x);
	if (a < 0.0f) a += Math::TAU;
	return static_cast<float>(a);
}

float vec2::dot(vec2 v) const
{
    return x * v.x + y * v.y;
}

float vec2::length() const
{
    return Math::sqrt(length_squared());
}

float vec2::length_squared() const {
	return x * x + y * y;
}

bool vec2::linearly_independent(const vec2& other) const { return *this != other.abs(); }

vec2 vec2::normalized() const { return vec2(*this) / length(); }

vec2::operator String() const {
	return "vec2(" + String(x) + ", " + String(y) + ")";
}

void vec2::register_to_db(vm::ClassDB& db) {
        const int16_t id = REGISTER_CLASS(vec2);
		
		REGISTER_CONSTRUCTOR(id, vec2());
		REGISTER_CONSTRUCTOR(id, vec2(float x, float y));

		REGISTER_SETGET(id, vec2, float, x);
		REGISTER_SETGET(id, vec2, float, y);

        REGISTER_METHOD(id, vec2, set_xx, void (vec2::*)(vec2 xx));
        REGISTER_METHOD(id, vec2, set_yx, void (vec2::*)(vec2 yx));
        REGISTER_METHOD(id, vec2, set_yy, void (vec2::*)(vec2 yy));

        REGISTER_METHOD(id, vec2, get_xx, vec2 (vec2::*)() const);
        REGISTER_METHOD(id, vec2, get_yx, vec2 (vec2::*)() const);
        REGISTER_METHOD(id, vec2, get_yy, vec2 (vec2::*)() const);

        REGISTER_METHOD(id, vec2, dot);
        REGISTER_METHOD(id, vec2, length_squared, float (vec2::*)() const);
        REGISTER_METHOD(id, vec2, length, float (vec2::*)() const);

        REGISTER_METHOD(id, vec2, operator+, vec2 (vec2::*)(const vec2& other) const);
        REGISTER_METHOD(id, vec2, operator-, vec2 (vec2::*)() const);
        REGISTER_METHOD(id, vec2, operator-, vec2 (vec2::*)(const vec2& other) const);
        REGISTER_METHOD(id, vec2, operator*);
        REGISTER_METHOD(id, vec2, operator/);
}

}