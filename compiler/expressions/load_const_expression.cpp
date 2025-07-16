#include "load_const_expression.hpp"

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
    return C_HIDE("L") + std::to_string(*reinterpret_cast<int64_t*>(value));
}

}