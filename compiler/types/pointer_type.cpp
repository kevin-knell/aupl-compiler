#include "pointer_type.hpp"

cmp::PointerType::PointerType(TypePtr type)
	: type(type) {}

std::string cmp::PointerType::to_string() const {
	return type->to_string() + "*";
}

int cmp::PointerType::get_size() const {
	return sizeof(void*);
}

cmp::Type::KIND cmp::PointerType::get_kind() const {
	return POINTER;
}

std::string cmp::PointerType::to_cpp_type_str() {
	return type->to_cpp_type_str() + "*";
}
