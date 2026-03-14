#include "unary_op_expression.hpp"
#include "type.hpp"
#include "color.hpp"

namespace cmp {

std::string UnaryExpression::to_string() const {
#ifdef DEBUG
    return C_PAREN(operator_to_string() + " " + expr->to_string());
#else
    return operator_to_string() + " " + expr->to_string();
#endif
}

std::vector<ExprPtr*> UnaryExpression::get_expressions() {
    return {&expr};
}

TypePtr UnaryExpression::get_type() const {
    return expr->get_type();
}

bool UnaryExpression::is_constexpr() const {
    return expr->is_constexpr();
}

vm::Value* UnaryExpression::eval_constexpr() const {
    vm::Value8* expr_val = reinterpret_cast<vm::Value8*>(expr->eval_constexpr());

    vm::Value8* result = new vm::Value8();

    switch (op) {
        case OPERATOR::NOT: result->i64 = !expr_val->i64; break;
        case OPERATOR::MINUS: result->i64 = -expr_val->i64; break;
    }

    return reinterpret_cast<vm::Value*>(result);
}

}