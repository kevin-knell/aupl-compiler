#pragma once
#include "object.hpp"
#include <string>

namespace vm {
    class ClassDB;
} // namespace vm

class String : public Object {
private:
    std::string value;
public:
	static void register_to_db(vm::ClassDB& db);
    
	String() = default;
	String(const String& s);
    String(const char* s);
    String(const std::string& s);
    String(std::string&& s);
	String(String&& other) noexcept;

    const std::string& str() const;
    size_t size() const;
    bool empty() const;
    char operator[](size_t i) const;
	String& operator=(const String& s) = default;
};