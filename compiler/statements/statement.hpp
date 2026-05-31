// statement.hpp
#pragma once
#include <memory>
#include <string>
#include <vector>
#include "function_symbol.hpp"
#include "forward_declarations.hpp"
#include "statement_visitor.hpp"

#define OVERRIDE_ACCEPT_STATMENT_VISITOR void accept(StatementVisitor& visitor) override { visitor.visit(*this); }
#define DEFINE_SELF_STMT(m_type) \
	std::shared_ptr<m_type> self() { return std::static_pointer_cast<m_type>(shared_from_this()); } \
	StmtPtr clone() const override { return std::make_shared<m_type>(*this); }

namespace cmp {

struct Statement : public std::enable_shared_from_this<Statement> {
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

	virtual StmtPtr clone() const = 0;
};

}