#include "string_literal_expression.hpp"
#include "string.hpp"

namespace cmp {

std::string cmp::StringLiteralExpression::to_string() const {
	return "\"" + value + "\"";
}

std::vector<uint8_t> StringLiteralExpression::generate_bytecode(BytecodeGenerationInfo &bgi) const {
	return {};
}

size_t StringLiteralExpression::get_bytecode_size(BytecodeGenerationInfo &bgi) const {
	return 0;
}

std::vector<ExprPtr *> StringLiteralExpression::get_expressions() {
	return {};
}
TypePtr StringLiteralExpression::get_type() const {
	return type;
}
bool StringLiteralExpression::is_constexpr() const {
	return true;
}
vm::Value* StringLiteralExpression::eval_constexpr() const {
	String* s = new String(value);
	return reinterpret_cast<vm::Value*>(s);
}
}