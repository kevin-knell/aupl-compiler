#pragma once
#include "statement.hpp"

namespace cmp
{

struct BlockStatement : Statement {
    ScopePtr scope;

    BlockStatement(ScopePtr scope) : scope(scope) {}
	
	OVERRIDE_ACCEPT_STATMENT_VISITOR

    KIND get_kind() const override;

    std::vector<ExprPtr*> get_expressions() override;

    std::string to_string() const override;
};

} // namespace cmp
