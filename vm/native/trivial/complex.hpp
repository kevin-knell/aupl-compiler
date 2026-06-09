#include <complex>
#include "native.hpp"

namespace vm {
    class ClassDB;
} // namespace vm

namespace auplib {

struct complex {
    static void register_to_db(vm::ClassDB& db);

    static const float ONE;
    static const float I;
	
	float real;
	float imag;

	constexpr complex() = default;
	constexpr complex(float r) : real(r), imag(0.0f) {}
	constexpr complex(float r, float i) : real(r), imag(i) {}

    float re() const { return real; }
    float im() const { return imag; }
};

MARK_TRIVIAL(complex)
static_assert(sizeof(complex) == sizeof(float) * 2);

}