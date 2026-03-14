// return_statement.hpp
#pragma once
#include "statement.hpp"
#include "expression.hpp"
#include "forward_declarations.hpp"

namespace cmp {

struct ReturnStatement : public Statement {
    ExprPtr return_value;

    ReturnStatement(ExprPtr val)
        : return_value(std::move(val)) {}

	OVERRIDE_ACCEPT_STATMENT_VISITOR

    std::vector<ExprPtr*> get_expressions() override;

    std::string to_string() const override;

    virtual KIND get_kind() const override { return RETURN; }
};

}