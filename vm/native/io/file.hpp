#pragma once
#include "native.hpp"
#include "object.hpp"
#include "shared.hpp"
#include "macros.hpp"
#include <fstream>

namespace vm {
    class ClassDB;
} // namespace vm

namespace auplib {

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

	static Shared<File> open(const String& path, int64_t open_flag);

	File() = default;
	File(const String& path, int64_t open_flag);
	~File();

	COPY_DELETE(File)
	MOVE_DEFAULT(File)

	// read
	String get_line();
	bool end() const;

	// write
	void write_line(const String& s);
};

}