// expression.hpp
#pragma once
#include "expression.hpp"

namespace cmp {

struct LoadConstExpression : Expression {
	TypePtr type;
	vm::Value* value;

	LoadConstExpression(TypePtr type, vm::Value* value) : type(type), value(value) {}

	OVERRIDE_ACCEPT_EXPRESSION_VISITOR

	std::string to_string() const override;
	std::vector<ExprPtr*> get_expressions() override;
	bool is_unresolved_symbol() const override { return false; }
	void resolve(NameAnalysisInfo& name_analysis_info) override {
		(void)name_analysis_info;
	};
	int get_level() override;
	TypePtr get_type() const override;
	KIND get_kind() const override { return LOAD_CONST; }
	bool is_constexpr() const override;
	vm::Value* eval_constexpr() const override;
};
using ExprPtr = std::shared_ptr<Expression>;

}