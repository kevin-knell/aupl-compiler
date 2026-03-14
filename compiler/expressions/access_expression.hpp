#pragma once
#include "expression.hpp"

namespace cmp
{

struct AccessExpression : public Expression {
    ExprPtr left;
    ExprPtr right;

    AccessExpression(ExprPtr left, ExprPtr right)
        : left(std::move(left)), right(std::move(right)) {}
	
	OVERRIDE_ACCEPT_EXPRESSION_VISITOR

    std::string to_string() const override;
    std::vector<ExprPtr*> get_expressions() override;
    bool is_unresolved_symbol() const override;
    void resolve(NameAnalysisInfo& name_analysis_info) override;
    TypePtr get_type() const override;
	KIND get_kind() const override { return ACCESS; }
};

} // namespace cmp
