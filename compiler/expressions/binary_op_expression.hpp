#pragma once
#include "expression.hpp"
#include "instructions.hpp"

namespace cmp {

struct BinaryExpression : public Expression {
    using OPERATOR = vm::BinOp;

    ExprPtr left;
    ExprPtr right;
    OPERATOR op;

    BinaryExpression(ExprPtr left, ExprPtr right, OPERATOR op)
        : left(left), right(right), op(op) {}
	
	OVERRIDE_ACCEPT_EXPRESSION_VISITOR

    std::string to_string() const override;
    std::vector<ExprPtr*> get_expressions() override;
    TypePtr get_type() const override;
    KIND get_kind() const override { return BINARY; }

    virtual bool is_constexpr() const override;
    virtual vm::Value* eval_constexpr() const override;

    const std::string operator_to_string() const {
        switch (op) {
            case OPERATOR::ADD: return "+";
            case OPERATOR::SUB: return "-";
            case OPERATOR::MUL: return "*";
            case OPERATOR::DIV: return "/";
            case OPERATOR::MOD: return "%";

            case OPERATOR::EQ:  return "==";
            case OPERATOR::NEQ: return "!=";
            case OPERATOR::GT:  return ">";
            case OPERATOR::LT:  return "<";
            case OPERATOR::GE:  return ">=";
            case OPERATOR::LE:  return "<=";

            case OPERATOR::AND: return "and";
            case OPERATOR::OR:  return "or";

            default: return "unknown";
        }
    }
};

}