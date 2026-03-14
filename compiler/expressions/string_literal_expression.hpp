#pragma once
#include "expression.hpp"

namespace cmp {

struct StringLiteralExpression : Expression {
	TypePtr type;
	std::string value;

	StringLiteralExpression(TypePtr type, std::string value) : type(type), value(value) {}

	OVERRIDE_ACCEPT_EXPRESSION_VISITOR

	std::string to_string() const override;
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