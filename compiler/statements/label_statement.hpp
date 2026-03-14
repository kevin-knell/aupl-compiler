#pragma once
#include "statement.hpp"

namespace cmp
{

struct LabelStatement : Statement {
    std::string identifier;

	LabelStatement(const std::string& id) : identifier(id) {}

	OVERRIDE_ACCEPT_STATMENT_VISITOR

    KIND get_kind() const override;

    std::vector<ExprPtr*> get_expressions() override;

    std::string to_string() const override;
};

} // namespace cmp
