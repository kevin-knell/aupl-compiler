// call_expression.hpp
#pragma once
#include "expression.hpp"
#include "function_symbol.hpp"

namespace cmp {

struct CallExpression final : public Expression {
    std::string name;
    std::vector<ExprPtr> arguments;
	ExprPtr obj_expr;
    FuncPtr f;

    CallExpression(std::string name, const std::vector<ExprPtr>& arguments, ExprPtr obj_expr)
        : name(name), arguments(arguments), obj_expr(obj_expr) {}
	
	CallExpression(FuncPtr f, const std::vector<ExprPtr>& arguments, ExprPtr obj_expr)
		: name(f->name), arguments(arguments), obj_expr(obj_expr), f(f) {}
	
	OVERRIDE_ACCEPT_EXPRESSION_VISITOR

    std::string to_string() const override;
    TypePtr get_type() const override;
    bool is_unresolved_symbol() const override;
    void resolve(NameAnalysisInfo& name_analysis_info) override;
    std::vector<ExprPtr*> get_expressions() override;
    bool is_pure() const override;
    KIND get_kind() const override { return CALL; }
};

}