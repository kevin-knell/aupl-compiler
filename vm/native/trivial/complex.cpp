#include "complex.hpp"

namespace auplib {

double complex::re() const {
    return value.real();
}

double complex::im() const {
    return value.imag();
}

}