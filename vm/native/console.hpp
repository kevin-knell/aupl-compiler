#pragma once
#include "iostream"
#include "class_db.hpp"
#include "native.hpp"
#include "object.hpp"
#include "string.hpp"


class Console {
public:
    DECLARE_UTILITY_CLASS(Console);

	static void register_to_db(vm::ClassDB& db);

    static void print();
    static void print(const String& text);
    static void print(const int64_t i);
    static void print(const double d);

    static String scan();
    static int scan_int();
    static double scan_double();
};

STATIC_ASSERT_UTILITY_CLASS(Console);