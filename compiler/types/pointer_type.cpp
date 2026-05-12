#include "pointer_type.hpp"

namespace cmp {

PointerType::PointerType(TypePtr type)
	: type(type) {}

std::string PointerType::to_string() const {
	return type->to_string() + "*";
}

size_t PointerType::get_size() const {
	return sizeof(void*);
}

Type::KIND PointerType::get_kind() const {
	return POINTER;
}

std::string PointerType::to_cpp_type_str() {
	return type->to_cpp_type_str() + "*";
}

}