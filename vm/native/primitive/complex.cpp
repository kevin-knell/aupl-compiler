#include "complex.hpp"


double complex::re() const {
    return value.real();
}

double complex::im() const {
    return value.imag();
}