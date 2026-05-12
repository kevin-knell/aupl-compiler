#include "native_class_type.hpp"
#include "class_db.hpp"

namespace cmp
{
std::string NativeClassType::to_string() const {
	return cls.name;
}

size_t NativeClassType::get_size() const {
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

std::string NativeClassType::to_cpp_type_str() {
	return cls.name;
}

bool NativeClassType::default_store_shared() const {
	return cls.is_object;
}

NativeClassType::NativeClassType(const vm::ClassBind& cls) : cls(cls) {}

} // namespace cmp
