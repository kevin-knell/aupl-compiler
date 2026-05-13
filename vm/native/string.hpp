#pragma once
#include <string>

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
	String(String&& other) noexcept;

    String(const char* s);
    String(const std::string& s);
    String(std::string&& s);
	String(int64_t i);
	String(void* p);

    const std::string& str() const;
    size_t size() const;
    bool empty() const;
    char operator[](size_t i) const;
	
	// operators
	String& operator=(const String& s) = default;
	
	operator std::string() const { return value; }
	operator const char*() const { return value.c_str(); }
};

}