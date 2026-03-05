#include "native_class_type.hpp"
#include "class_db.hpp"

namespace cmp
{
std::string NativeClassType::to_string() const {
	return cls.name;
}

int NativeClassType::get_size() const {
	return cls.size;
}

Type::KIND NativeClassType::get_kind() const {
	return NATIVE_CLASS;
}

bool NativeClassType::is_cpp_type(const std::string &cpp_type) const {
	return cls.name == cpp_type
			|| ("const " + cls.name) == cpp_type
			|| (cls.name + "&") == cpp_type
			|| ("const " + cls.name + "&") == cpp_type;
}

NativeClassType::NativeClassType(const vm::ClassBind& cls) : cls(cls) {}

} // namespace cmp
