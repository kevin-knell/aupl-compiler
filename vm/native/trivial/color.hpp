#pragma once

#include "native.hpp"
#include <cstdint>

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
		uint8_t r_u8 = r * 255;
		uint8_t g_u8 = g * 255;
		uint8_t b_u8 = b * 255;
		uint8_t a_u8 = a * 255;

		return (r_u8 << 24) | (g_u8 << 16) | (b_u8 << 8) | (a_u8 << 0);
	}
};

MARK_TRIVIAL(Color)

}