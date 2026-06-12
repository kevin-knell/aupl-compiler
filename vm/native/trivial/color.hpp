#pragma once

#include "native.hpp"
#include <cstdint>
#include "macros.hpp"

namespace vm {
	class ClassDB;
}

namespace auplib {

struct Color {
	float r, g, b, a;

	static void register_to_db(vm::ClassDB& db);

	constexpr Color() = default;
	constexpr Color(float v) : r(v), g(v), b(v), a(1.0f) {}
	constexpr Color(float r, float g, float b, float a = 1.0f) : r(r), g(g), b(b), a(a) {}

	uint32_t get_r8g8b8a8_unorm() const {
		uint8_t r_u8 = static_cast<uint8_t>(r * 255.0);
		uint8_t g_u8 = static_cast<uint8_t>(g * 255.0);
		uint8_t b_u8 = static_cast<uint8_t>(b * 255.0);
		uint8_t a_u8 = static_cast<uint8_t>(a * 255.0);

		return (r_u8 << 24) | (g_u8 << 16) | (b_u8 << 8) | (a_u8 << 0);
	}

	SETGET(r)
	SETGET(g)
	SETGET(b)
	SETGET(a)
};

MARK_TRIVIAL(Color)

}