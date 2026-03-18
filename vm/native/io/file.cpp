#include "file.hpp"
#include <string>
#include "native.hpp"
#include "class_db.hpp"
#include "string.hpp"

void File::register_to_db(vm::ClassDB &db) {
	const int ID = REGISTER_CLASS(ID, File);
	
	//REGISTER_CONSTRUCTOR(ID, File(const String& path, int64_t open_flag));

	REGISTER_STATIC_METHOD(ID, File, open, File (*)(const String& path, int64_t open_flag));

	REGISTER_METHOD(ID, File, get_line, String (File::*)());
	REGISTER_METHOD(ID, File, end, bool (File::*)() const);

	REGISTER_METHOD(ID, File, write_line, void (File::*)(const String& s));
}

File::File(const String& path, int64_t open_flag) {
	std::ios::openmode open_mode;
	if (
		open_flag &
		(
			static_cast<int64_t>(OPEN_FLAG::WRITE) |
			static_cast<int64_t>(OPEN_FLAG::READ)
		)
	) open_mode = std::ios::out | std::ios::in;
	else if (
		open_flag &
		static_cast<int64_t>(OPEN_FLAG::READ)
	) open_mode = std::ios::in;
	else if (
		open_flag &
		static_cast<int64_t>(OPEN_FLAG::WRITE)
	) open_mode = std::ios::out;
	else {
		std::cerr << "Error: Invalid open flag!" << std::endl;
		return;
	}

	file_access.open(path.str(), open_mode);
}

File File::open(const String& path, int64_t open_flag) {
	return File(path, open_flag);
}

File::~File() {
	file_access.close();
}

String File::get_line() {
	std::string s;
	std::getline(file_access, s);
	return String(s);
}

bool File::end() const {
	return file_access.eof();
}

void File::write_line(const String& s) {
	file_access << s.str() << std::flush;
}
