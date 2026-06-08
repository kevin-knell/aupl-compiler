// expression.hpp
#pragma once
#include "expression.hpp"
#include "call_expression.hpp"
#include "tuple_expression.hpp"

namespace cmp {

struct NodeCompositionExpression final : public Expression {
	std::string name;
	ExprPtr constructor_call_expr;
	ExprPtr content_tuple_expr;

	NodeCompositionExpression(std::string name, ExprVec args, ExprVec content)
		:	name(name),
			constructor_call_expr(std::make_shared<CallExpression>(name, args, nullptr)),
			content_tuple_expr(std::make_shared<TupleExpression>(content)) {}

	OVERRIDE_ACCEPT_EXPRESSION_VISITOR
	DEFINE_SELF_EXPR(NodeCompositionExpression)

	std::string to_string() const override;
	std::vector<ExprPtr*> get_expressions() override;
	bool is_unresolved_symbol() const override;
	TypePtr get_type() const override;
	KIND get_kind() const override { return LOAD_CONST; }
	bool is_constexpr() const override;
	vm::Value* eval_constexpr() const override;
};
using ExprPtr = std::shared_ptr<Expression>;

}