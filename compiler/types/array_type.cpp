#include "array_type.hpp"

namespace cmp {

std::string ArrayType::to_string() const {
	return base_type->to_string()
			+ "["
			+ (is_const ? "const " : "")
			+ (size_expr ? size_expr->to_string() : "")
			+ "]";
}

}