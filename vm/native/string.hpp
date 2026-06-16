#pragma once
#include <string>
#include "macros.hpp"

namespace vm {
    class ClassDB;
} // namespace vm

namespace auplib {

class String {
private:
    std::string value;
public:
	static void register_to_db(vm::ClassDB& db);
    
	String() = default;

	String(const String& s);
	String& operator=(const String& s);

	String(String&& other) noexcept;
	String& operator=(String&& s);

    String(const char* s);
    String(const std::string& s);
    String(std::string&& s);

	template<typename T>
	String(T val) : value(std::to_string(val)) {}

    const std::string& str() const;
    size_t size() const;
    bool empty() const;
    char operator[](size_t i) const;
	
	// operators
	String operator+(const String& s);
	String operator+(const char* s);
	
	operator std::string() const { return value; }
	operator const char*() const { return value.c_str(); }
};

String operator+(const char* s1, const String& s2);
std::ostream& operator<<(std::ostream& os, const String& s);

}