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
	
	OVERRIDE_ACCEPT_STATMENT_VISITOR

    std::vector<ExprPtr*> get_expressions() override {
        return {&expression};
    }

    KIND get_kind() const override { return EXPRESSION; }
};

}