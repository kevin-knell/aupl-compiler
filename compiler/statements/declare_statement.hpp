#pragma once
#include "statement.hpp"
#include "expression.hpp"
#include "type.hpp"

namespace cmp {

struct DeclareStatement : public Statement {
    VarPtr variable_symbol;

    DeclareStatement(const VarPtr& variable_symbol);

    std::string to_string() const override;

	OVERRIDE_ACCEPT_STATMENT_VISITOR

    std::vector<ExprPtr*> get_expressions() override;

    virtual KIND get_kind() const override { return DECLARE; }
};

} // namespace cmp
