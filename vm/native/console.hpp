#pragma once
#include "iostream"
#include "native.hpp"
#include "object.hpp"

class Console {
public:
    DECLARE_UTILITY_CLASS(Console);

    static void print(const std::string& text);

    static int scan();
};

STATIC_ASSERT_UTILITY_CLASS(Console);