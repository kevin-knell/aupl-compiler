#pragma once
#include "native.hpp"
#include "object.hpp"
#include <fstream>

namespace vm {
    class ClassDB;
} // namespace vm

class String;

class File : Object {
private:
	std::fstream file_access;
public:
	enum class OPEN_FLAG : int64_t {
		READ = 1,
		WRITE = 2
	};

	static void register_to_db(vm::ClassDB& db);

	static File open(const String& path, int64_t open_flag);

	File() = default;
	File(const String& path, int64_t open_flag);
	~File();


    File(const File&) = delete;
    File& operator=(const File&) = delete;

    File(File&&) noexcept = default;
    File& operator=(File&&) noexcept = default;

	// read
	String get_line();
	bool end() const;

	// write
	void write_line(const String& s);
};
