#pragma once
#include "expression.hpp"

namespace cmp {

struct StringLiteralExpression : Expression {
	TypePtr type;
	std::string value;

	StringLiteralExpression(TypePtr type, std::string value) : type(type), value(value) {}

	std::string to_string() const override;
	std::vector<uint8_t> generate_bytecode(BytecodeGenerationInfo& bgi) const override;
	size_t get_bytecode_size(BytecodeGenerationInfo& bgi) const override;
	std::vector<ExprPtr*> get_expressions() override;
	bool is_unresolved_symbol() const override { return false; }
	void resolve(NameAnalysisInfo&) override {};
	int get_level() override { return 1; };
	TypePtr get_type() const override;
	KIND get_kind() const override { return STRING_LIT; }
	bool is_constexpr() const override;
	vm::Value* eval_constexpr() const override;
};
using ExprPtr = std::shared_ptr<Expression>;

}