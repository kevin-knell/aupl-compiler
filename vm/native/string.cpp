#include "string.hpp"
#include "class_db.hpp"
#include "native.hpp"
#include <iostream>

String::String(const String &s) : value(s.str()) {}

String::String(const char *s) : value(s) {}
String::String(const std::string& s) : value(s) {}
String::String(std::string&& s) : value(std::move(s)) {}
String::String(String &&other) noexcept : value(std::move(other.value)) {}
String::String(int64_t i) : value(std::to_string(i)) {}
//String::String(void *p) : value(std::to_string(size_t(p))) {}

const std::string& String::str() const { return value; }
size_t String::size() const { return value.size(); }
bool String::empty() const { return value.empty(); }
char String::operator[](size_t i) const { return value[i]; }

void String::register_to_db(vm::ClassDB& db) {
        const int id = REGISTER_CLASS(id, String);

		REGISTER_CONSTRUCTOR(id, String());
		REGISTER_CONSTRUCTOR(id, String(const char* s));
		REGISTER_CONSTRUCTOR(id, String(int64_t i));

		REGISTER_METHOD(id, String, size, size_t (String::*)() const);
}