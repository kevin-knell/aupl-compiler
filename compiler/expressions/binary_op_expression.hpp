#pragma once
#include "expression.hpp"

namespace cmp {

struct BinaryExpression : public Expression {
    enum class OPERATOR {
        ADD,
        SUB,
        MUL,
        DIV,
        MOD,

        EQ,
        NEQ,
        GT,
        LT,
        GE,
        LE,

        AND,
        OR
    };
    
    ExprPtr left;
    ExprPtr right;
    OPERATOR op;

    BinaryExpression(ExprPtr left, ExprPtr right, OPERATOR op)
        : left(std::move(left)), right(std::move(right)), op(op) {}

    std::string to_string() const override;
    std::vector<uint8_t> generate_bytecode(BytecodeGenerationInfo& bgi) const override;
    size_t get_bytecode_size(BytecodeGenerationInfo& bgi) const override;
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

            case OPERATOR::AND: return "&&";
            case OPERATOR::OR:  return "||";

            default: return "unknown";
        }
    }
};

}