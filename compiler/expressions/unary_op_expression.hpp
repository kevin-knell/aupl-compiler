#pragma once
#include "expression.hpp"
#include "instructions.hpp"

namespace cmp {

struct UnaryExpression : public Expression {
    enum OPERATOR {
		NOT,
		MINUS,

		NONE
	};

    ExprPtr expr;
    OPERATOR op;

    UnaryExpression(ExprPtr expr, OPERATOR op)
        : expr(expr), op(op) {}
	
	OVERRIDE_ACCEPT_EXPRESSION_VISITOR

    std::string to_string() const override;
    std::vector<ExprPtr*> get_expressions() override;
    TypePtr get_type() const override;
    KIND get_kind() const override { return UNARY; }

    virtual bool is_constexpr() const override;
    virtual vm::Value* eval_constexpr() const override;

    const std::string operator_to_string() const {
        switch (op) {
            case OPERATOR::NOT: return "not";
            case OPERATOR::MINUS: return "-";

            default: return "unknown";
        }
    }
};

}