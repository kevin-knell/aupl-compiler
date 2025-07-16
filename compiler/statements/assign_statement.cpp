#include "assign_statement.hpp"
#include "instructions.hpp"
#include "primitive_type.hpp"

namespace cmp {

AssignmentStatement::AssignmentStatement(ExprPtr expr_left, ExprPtr expr_right)
    : expr_left(std::move(expr_left)), expr_right(std::move(expr_right)) {}

std::string AssignmentStatement::to_string() const {
    return expr_left->to_string() + " = " + expr_right->to_string();
}

std::vector<uint8_t> AssignmentStatement::generate_bytecode(BytecodeGenerationInfo& bgi) const {
    return { static_cast<uint8_t>(vm::Instruction::ERR) };
}

size_t AssignmentStatement::get_bytecode_size(BytecodeGenerationInfo &bgi) const {
    return 1;
}

std::vector<ExprPtr*> AssignmentStatement::get_expressions() {
    std::vector<ExprPtr*> result = {&expr_left, &expr_right};

    auto result_left = expr_left->get_expressions();
    result.insert(result.end(), result_left.begin(), result_left.end());

    auto result_right = expr_right->get_expressions();
    result.insert(result.end(), result_right.begin(), result_right.end());

    return result;
}

} // namespace cmp
