#include "console.hpp"

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

void Console::register_to_db(vm::ClassDB& db) {
		constexpr int id = 2;

		REGISTER_CLASS(id, Console);

		REGISTER_GLOBAL_METHOD(id, Console, print, void (*)());
		REGISTER_GLOBAL_METHOD(id, Console, print, void (*)(const String& text));
		//REGISTER_GLOBAL_METHOD(id, Console, print, void (*)(const int64_t i));
		//REGISTER_GLOBAL_METHOD(id, Console, print, void (*)(const double d));

		REGISTER_GLOBAL_METHOD(id, Console, scan, String (*)());
		REGISTER_GLOBAL_METHOD(id, Console, scan_int, int (*)());
		REGISTER_GLOBAL_METHOD(id, Console, scan_double, double (*)());
}