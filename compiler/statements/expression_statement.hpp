// expression_statement.hpp
#pragma once
#include "statement.hpp"
#include "expression.hpp"

namespace cmp {

struct ExpressionStatement : public Statement {
    ExprPtr expression;

    ExpressionStatement(ExprPtr expr)
        : expression(std::move(expr)) {}

    std::string to_string() const override {
        return expression->to_string();
    }

    std::vector<uint8_t> generate_bytecode(BytecodeGenerationInfo& bgi) const override {
        return expression->generate_bytecode(bgi);
    }
    virtual size_t get_bytecode_size(BytecodeGenerationInfo& bgi) const override {
        return expression->get_bytecode_size(bgi);
    }

    std::vector<ExprPtr*> get_expressions() override {
        return {&expression};
    }

    virtual KIND get_kind() const override { return EXPRESSION; }
};

}