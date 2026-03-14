// statement.hpp
#pragma once
#include <memory>
#include <string>
#include <vector>
#include "function_symbol.hpp"
#include "forward_declarations.hpp"
#include "statement_visitor.hpp"

#define OVERRIDE_ACCEPT_STATMENT_VISITOR void accept(StatementVisitor& visitor) override { visitor.visit(*this); }

namespace cmp {

struct Statement {
    enum KIND {
        DECLARE,
        ASSIGN,
        EXPRESSION,
        BLOCK,
        IF,
        RETURN,
        LABEL
    };

	bool is_volatile = false;

    virtual ~Statement() = default;

	virtual void accept(StatementVisitor& visitor) { visitor.visit(*this); }

    virtual std::vector<ExprPtr*> get_expressions() { return {}; };

    virtual KIND get_kind() const = 0;

    virtual std::string to_string() const = 0;
};

using StmtPtr = std::shared_ptr<Statement>;

}