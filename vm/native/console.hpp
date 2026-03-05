#pragma once
#include "class_db.hpp"
#include "native.hpp"
#include "object.hpp"
#include "string.hpp"
#include "vec2.hpp"


class Console {
public:
    DECLARE_UTILITY_CLASS(Console);

	static void register_to_db(vm::ClassDB& db);

    static void print();
    static void print(const String& text);
    static void print(const int64_t i);
    static void print(const double d);
	static void print(const vec2 v);

    static String scan();
    static int scan_int();
    static double scan_double();

	// ncurses
	static void initscr();
	static void endwin();
	static void clear();
	static void move(int64_t x, int64_t y);
	static void printw(String s);
	static void refresh();
	static int64_t get_char();
};

STATIC_ASSERT_UTILITY_CLASS(Console);