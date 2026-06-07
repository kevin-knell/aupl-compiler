#include "expression.hpp"

#include "forward_declarations.hpp"

namespace cmp {
	
struct TupleExpression final : public Expression {
	ExprVec expressions;

	TupleExpression(ExprVec expressions) : expressions(expressions) {}

	OVERRIDE_ACCEPT_EXPRESSION_VISITOR
	DEFINE_SELF_EXPR(TupleExpression)
	
    std::vector<ExprPtr*> get_expressions() override;
	virtual bool is_unresolved_symbol() const override;

    std::string to_string() const override;
    TypePtr get_type() const override;
    KIND get_kind() const override { return TUPLE; }
    bool is_constexpr() const override;
    vm::Value* eval_constexpr() const override;

};

} // namespace cmp
