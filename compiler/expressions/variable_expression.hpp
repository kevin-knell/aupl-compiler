#pragma once
#include "expression.hpp"
#include "variable_symbol.hpp"
#include "class_db.hpp"

namespace cmp {

struct VariableExpression final : public Expression {
	ExprPtr obj_expr;
    VarPtr var;
    std::string name;
	bool must_be_dereferenced = false;
	bool is_lvalue = false;

    VariableExpression(const std::string& name) : name(name) {}
    VariableExpression(VarPtr var) : var(var), name(var->name) {}

	OVERRIDE_ACCEPT_EXPRESSION_VISITOR
	DEFINE_SELF_EXPR(VariableExpression)
	
    std::vector<ExprPtr*> get_expressions() override;

    std::string to_string() const override;
    bool is_unresolved_symbol() const override;
    TypePtr get_type() const override;
    bool is_constexpr() const override;
    vm::Value* eval_constexpr() const override;
    KIND get_kind() const override { return VARIABLE; }
};

using VarExprPtr = std::shared_ptr<VariableExpression>;

}