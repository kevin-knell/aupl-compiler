#include "math.hpp"
#include <cmath>
#include <algorithm>

double Math::sqrt(double x) {
    return std::sqrt(x);
}

// pow
double Math::pow(double b, double e) {
    return std::pow(b, e);
}

double Math::exp(double x) {
    return std::exp(x);
}

double Math::ln(double x) {
    return std::log(x);
}

double Math::log(double b, double x) {
    return std::log(x) / std::log(b);
}

double Math::log2(double x) {
    return std::log2(x);
}

double Math::log10(double x) {
    return std::log10(x);
}

// trigonometry
double Math::sin(double x) {
    return std::sin(x);
}

double Math::cos(double x) {
    return std::cos(x);
}

double Math::tan(double x) {
    return std::tan(x);
}

double Math::asin(double x) {
    return std::asin(x);
}

double Math::acos(double x) {
    return std::acos(x);
}

double Math::atan(double x) {
    return std::atan(x);
}

// range
double Math::max(double a, double b) {
    return std::max(a, b);
}

long Math::max(int a, int b) {
    return std::max(a, b);
}

double Math::min(double a, double b) {
    return std::min(a, b);
}

int Math::min(int a, int b) {
    return std::min(a, b);
}

double Math::wrap(double x, double from, double to) {
    double range = to - from;
    return fmod(fmod(x - from, range) + range, range) + from;
}

int Math::wrap(int x, int from, int to) {
    int range = to - from;
    return ((x - from) % range + range) % range + from;
}

double Math::clamp(double x, double from, double to) {
    return std::clamp(x, from, to);
}

int Math::clamp(int x, int from, int to) {
    return std::clamp(x, from, to);
}
