#include "expression.hpp"

#include "forward_declarations.hpp"

namespace cmp {
	
struct TupleExpression : public Expression {
	ExprVec expressions;

	TupleExpression(ExprVec expressions) : expressions(expressions) {}
	
    std::vector<ExprPtr*> get_expressions() override;

    std::string to_string() const override;
    TypePtr get_type() const override;
    KIND get_kind() const override { return INT_LIT; }
    bool is_constexpr() const override;
    vm::Value* eval_constexpr() const override;

};

} // namespace cmp
