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
		uint8_t r_u8 = static_cast<uint8_t>(r * 255.0);
		uint8_t g_u8 = static_cast<uint8_t>(g * 255.0);
		uint8_t b_u8 = static_cast<uint8_t>(b * 255.0);
		uint8_t a_u8 = static_cast<uint8_t>(a * 255.0);

		return (r_u8 << 24) | (g_u8 << 16) | (b_u8 << 8) | (a_u8 << 0);
	}

	void set_r(float _r) { r = _r; }
	float get_r() const { return r; }

	void set_g(float _g) { g = _g; }
	float get_g() const { return g; }

	void set_b(float _b) { b = _b; }
	float get_b() const { return b; }

	void set_a(float _a) { a = _a; }
	float get_a() const { return a; }
};

MARK_TRIVIAL(Color)

}