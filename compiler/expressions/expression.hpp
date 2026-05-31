// expression.hpp
#pragma once
#include <memory>
#include <string>
#include <vector>
#include <cstdint>
#include "instructions.hpp"
#include "name_analysis_info.hpp"
#include "forward_declarations.hpp"
#include "expression_visitor.hpp"
#include <value.hpp>

#define OVERRIDE_ACCEPT_EXPRESSION_VISITOR \
	void accept(ExpressionVisitor& visitor) override { visitor.visit(*this); } \
	void accept(ExpressionAssignmentVisitor& visitor, VarExprPtr var_expr) override { visitor.visit(*this, var_expr); }

#define DEFINE_SELF_EXPR(m_type) \
	std::shared_ptr<m_type> self() { return std::static_pointer_cast<m_type>(shared_from_this()); } \
	ExprPtr clone() const override { return std::make_shared<m_type>(*this); }


namespace cmp {

struct Expression : public std::enable_shared_from_this<Expression> {
    enum KIND {
        LOAD_CONST,

        INT_LIT,
        DOUBLE_LIT,
        STRING_LIT,

        VARIABLE,
        CALL,

        ACCESS,
		INDEX,

        UNARY,
        BINARY,
        TERNARY,

		TUPLE
    };

    virtual ~Expression() = default;

    virtual std::string to_string() const = 0;

	virtual void accept(ExpressionVisitor& visitor) { visitor.visit(*this); }
	virtual void accept(ExpressionAssignmentVisitor& visitor, VarExprPtr var_expr) { visitor.visit(*this, var_expr); }
    
    virtual std::vector<ExprPtr*> get_expressions();
    virtual int get_level();
    
    virtual bool is_unresolved_symbol() const { return false; }
    virtual void resolve(NameAnalysisInfo& name_analysis_info) {
        (void)name_analysis_info;
    };
    
    virtual TypePtr get_type() const = 0;

    virtual bool is_constexpr() const { return false; };
    virtual vm::Value* eval_constexpr() const { return nullptr; };

    virtual bool is_pure() const { return is_constexpr(); };
    virtual vm::Value* eval_pure() const { return is_constexpr() ? eval_constexpr() : nullptr; };

    virtual KIND get_kind() const = 0;

	virtual ExprPtr clone() const = 0;
};
using ExprPtr = std::shared_ptr<Expression>;

}