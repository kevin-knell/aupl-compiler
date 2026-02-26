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

NativeClassType::NativeClassType(const vm::ClassBind& cls) : cls(cls) {}

} // namespace cmp
