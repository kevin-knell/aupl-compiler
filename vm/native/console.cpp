#include "console.hpp"
#include <iostream>
#include <ncurses.h>

void Console::print(){
	print("");
}

void Console::print(const String& text){
    std::cout << text.str() << std::endl;
}

void Console::print(const int64_t i){
    print(std::to_string(i));
}

void Console::print(const double d){
    print(std::to_string(d));
}

void Console::print(const vec2 v) {
	print("(" + std::to_string(v.x) + ", " + std::to_string(v.y) + ")");
}

String Console::scan() {
    std::string s;
    std::cin >> s;
	return String(s);
}

int Console::scan_int() {
    int i;
    std::cin >> i;
    return i;
}

double Console::scan_double() {
    double d;
    std::cin >> d;
    return d;
}


void Console::initscr() {
	::initscr();
}

void Console::endwin() {
	::endwin();
}

void Console::clear() {
	::clear();
}

void Console::move(int64_t x, int64_t y) {
	::move(y, x);
}

void Console::printw(String s) {
	::printw("%s", s.str().c_str());
}

void Console::refresh() {
	::refresh();
}

int64_t Console::get_char() {
	return getch();
}

void Console::register_to_db(vm::ClassDB& db) {
		constexpr int id = 2;

		REGISTER_CLASS(id, Console);

		REGISTER_GLOBAL_METHOD(id, Console, print, void (*)());
		REGISTER_GLOBAL_METHOD(id, Console, print, void (*)(const String& text));
		REGISTER_GLOBAL_METHOD(id, Console, print, void (*)(const int64_t i));
		REGISTER_GLOBAL_METHOD(id, Console, print, void (*)(const double d));
		REGISTER_GLOBAL_METHOD(id, Console, print, void (*)(const vec2 v));

		REGISTER_GLOBAL_METHOD(id, Console, scan, String (*)());
		REGISTER_GLOBAL_METHOD(id, Console, scan_int, int (*)());
		REGISTER_GLOBAL_METHOD(id, Console, scan_double, double (*)());

		REGISTER_STATIC_METHOD(id, Console, initscr, void (*)());
		REGISTER_STATIC_METHOD(id, Console, endwin, void (*)());
		REGISTER_STATIC_METHOD(id, Console, clear, void (*)());
		REGISTER_STATIC_METHOD(id, Console, move, void (*)(int64_t x, int64_t y));
		REGISTER_STATIC_METHOD(id, Console, printw, void (*)(String s));
		REGISTER_STATIC_METHOD(id, Console, refresh, void (*)());
		REGISTER_STATIC_METHOD(id, Console, get_char, int64_t (*)());
}