#include "shared_type.hpp"
#include "shared.hpp"

namespace cmp {


SharedType::SharedType(TypePtr type)
	: type(type) {}

std::string SharedType::to_string() const {
	return "Shared<" + type->to_string() + ">";
}

size_t SharedType::get_size() const {
	return sizeof(Shared<vm::Value>);
}

Type::KIND SharedType::get_kind() const {
	return SHARED;
}

std::string SharedType::to_cpp_type_str() {
	return "Shared<" + type->to_cpp_type_str() + ">";
}

} // namespace cmp