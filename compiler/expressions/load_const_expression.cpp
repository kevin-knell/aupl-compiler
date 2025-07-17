#include "load_const_expression.hpp"

#include "type.hpp"
#include "primitive_type.hpp"
#include "color.hpp"
#include <assert.h>

namespace cmp {

std::vector<uint8_t> LoadConstExpression::generate_bytecode(BytecodeGenerationInfo& bgi) const {
    return {};
}

size_t LoadConstExpression::get_bytecode_size(BytecodeGenerationInfo& bgi) const {
    return 0;
}

std::vector<ExprPtr*> LoadConstExpression::get_expressions() {
    return {};
}

int LoadConstExpression::get_level() {
    return 1;
}

TypePtr LoadConstExpression::get_type() const {
    return type;
}

bool LoadConstExpression::is_constexpr() const {
    return true;
}

vm::Value* LoadConstExpression::eval_constexpr() const {
    return value;
}

std::string LoadConstExpression::to_string() const {
	if (type == PrimitiveType::TYPE_BOOL) {
		if (value->as<bool>()) {
			return C_KEYWORD("true");
		} else {
			return C_KEYWORD("false");
		}
	}

	switch (type->get_size()) {
	case 1: return C_HIDE("L") + std::to_string(*reinterpret_cast<int8_t*>(value));
	case 2: return C_HIDE("L") + std::to_string(*reinterpret_cast<int16_t*>(value));
	case 4: return C_HIDE("L") + std::to_string(*reinterpret_cast<int32_t*>(value));
	case 8: return C_HIDE("L") + std::to_string(*reinterpret_cast<int64_t*>(value));
	default: return C_HIDE("L") + "unknown size";
	}
}

}