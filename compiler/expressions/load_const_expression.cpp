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

	if (type == PrimitiveType::TYPE_I8) {
		return C_HIDE("L") + std::to_string(*reinterpret_cast<int8_t*>(value));
	} else if (type == PrimitiveType::TYPE_I16) {
		return C_HIDE("L") + std::to_string(*reinterpret_cast<int16_t*>(value));
	} else if (type == PrimitiveType::TYPE_I32) {
		return C_HIDE("L") + std::to_string(*reinterpret_cast<int32_t*>(value));
	} else if (type == PrimitiveType::TYPE_INT) {
		return C_HIDE("L") + std::to_string(*reinterpret_cast<int64_t*>(value));
	}
	
	if (type == PrimitiveType::TYPE_U8) {
		return C_HIDE("L") + std::to_string(*reinterpret_cast<uint8_t*>(value));
	} else if (type == PrimitiveType::TYPE_U16) {
		return C_HIDE("L") + std::to_string(*reinterpret_cast<uint16_t*>(value));
	} else if (type == PrimitiveType::TYPE_U32) {
		return C_HIDE("L") + std::to_string(*reinterpret_cast<uint32_t*>(value));
	} else if (type == PrimitiveType::TYPE_U64) {
		return C_HIDE("L") + std::to_string(*reinterpret_cast<uint64_t*>(value));
	}
	
	if (type == PrimitiveType::TYPE_F32) {
		return C_HIDE("L") + std::to_string(*reinterpret_cast<float*>(value));
	} else if (type == PrimitiveType::TYPE_FLOAT) {
		return C_HIDE("L") + std::to_string(*reinterpret_cast<double*>(value));
	}
	
	if (type->get_size() == 0) {
		return "()";
	}

	return C_ERROR("L" + type->to_string());
}

}