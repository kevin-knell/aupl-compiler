#include "binary_op_expression.hpp"
#include "type.hpp"
#include "color.hpp"

namespace cmp {

std::vector<uint8_t> BinaryExpression::generate_bytecode(BytecodeGenerationInfo& bgi) const {
    return {};
}

size_t BinaryExpression::get_bytecode_size(BytecodeGenerationInfo &bgi) const {
    return 0;
}

std::string BinaryExpression::to_string() const {
#ifdef DEBUG
    return C_PAREN(left->to_string() + " " + operator_to_string() + " " + right->to_string());
#else
    return left->to_string() + " " + operator_to_string() + " " + right->to_string();
#endif
}

std::vector<ExprPtr*> BinaryExpression::get_expressions() {
    return {&left, &right};
}

TypePtr BinaryExpression::get_type() const {
    TypePtr left_type = left->get_type();
    TypePtr right_type = right->get_type();

    if (left_type == right_type) {
        return left_type;
    } else {
        return left_type; // TODO
    }
}

bool BinaryExpression::is_constexpr() const {
    return left->is_constexpr() && right->is_constexpr();
}

vm::Value* BinaryExpression::eval_constexpr() const {
    vm::Value8* left_val = reinterpret_cast<vm::Value8*>(left->eval_constexpr());
    vm::Value8* right_val = reinterpret_cast<vm::Value8*>(right->eval_constexpr());

    vm::Value8* result = new vm::Value8();

    switch (op) {
        case OPERATOR::ADD: result->i64 = left_val->i64 + right_val->i64; break;
        case OPERATOR::SUB: result->i64 = left_val->i64 - right_val->i64; break;
        case OPERATOR::MUL: result->i64 = left_val->i64 * right_val->i64; break;
        case OPERATOR::DIV: result->i64 = left_val->i64 / right_val->i64; break;
    }

    return reinterpret_cast<vm::Value*>(result);
}

}