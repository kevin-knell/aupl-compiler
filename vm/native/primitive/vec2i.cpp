#include "vec2i.hpp"
#include "class_db.hpp"
#include "native.hpp"

const vec2i vec2i::ZERO(0, 0);
const vec2i vec2i::ONE(0, 0);
const vec2i vec2i::EX(1, 0);
const vec2i vec2i::EY(0, 1);

void vec2i::register_to_db(vm::ClassDB &db) {
    const int16_t id = REGISTER_CLASS(vec2i);
	
	REGISTER_CONSTRUCTOR(id, vec2i());
	REGISTER_CONSTRUCTOR(id, vec2i(int64_t x, int64_t y));

	REGISTER_VARIABLE(id, int64_t, x);
	REGISTER_VARIABLE(id, int64_t, y);
}