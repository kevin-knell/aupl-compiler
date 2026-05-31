#pragma once
#include "expression.hpp"

namespace cmp {

struct IndexExpression final : public Expression {
	ExprPtr left_expr;
	ExprPtr index_expr;

    IndexExpression(ExprPtr left_expr, ExprPtr index_expr) : left_expr(left_expr), index_expr(index_expr) {}

	OVERRIDE_ACCEPT_EXPRESSION_VISITOR
	DEFINE_SELF_EXPR(IndexExpression)
	
    std::vector<ExprPtr*> get_expressions() override;

    std::string to_string() const override;
    bool is_unresolved_symbol() const override;
    void resolve(NameAnalysisInfo& name_analysis_info) override;
    TypePtr get_type() const override;
    bool is_constexpr() const override;
    vm::Value* eval_constexpr() const override;
    KIND get_kind() const override { return INDEX; }
};

using VarExprPtr = std::shared_ptr<VariableExpression>;

}