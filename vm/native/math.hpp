#pragma once
#include "native.hpp"


class Math {
public:
    DECLARE_UTILITY_CLASS(Math)
    
    static constexpr double TAU = 6.28318530717958647692;
    static constexpr double PI  = 3.14159265358979323846;
    static constexpr double E   = 2.71828182845904523536;

    static double sqrt(double x);

    // power
    static double pow(double b, double e);
    static double exp(double x);

    static double ln(double x);
    static double log(double b, double x);
    static double log2(double x);
    static double log10(double x);

    // trigonometry
    static double sin(double x);
    static double cos(double x);
    static double tan(double x);

    static double asin(double x);
    static double acos(double x);
    static double atan(double x);

    // range
    static double max(double a, double b);
    static long max(int a, int b);

    static double min(double a, double b);
    static int min(int a, int b);
    
    double wrap(double x, double from, double to);
    int wrap(int x, int from, int to);

    static double clamp(double x, double from, double to);
    static int clamp(int x, int from, int to);
};
static_assert(sizeof(long) == 8);

STATIC_ASSERT_UTILITY_CLASS(Math);