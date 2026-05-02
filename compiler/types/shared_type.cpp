#include "shared_type.hpp"
#include "shared.hpp"

cmp::SharedType::SharedType(TypePtr type)
	: type(type) {}

std::string cmp::SharedType::to_string() const {
	return "Shared<" + type->to_string() + ">";
}

int cmp::SharedType::get_size() const {
	return sizeof(Shared<vm::Value>);
}

cmp::Type::KIND cmp::SharedType::get_kind() const {
	return SHARED;
}

std::string cmp::SharedType::to_cpp_type_str() {
	return "Shared<" + type->to_cpp_type_str() + ">";
}
