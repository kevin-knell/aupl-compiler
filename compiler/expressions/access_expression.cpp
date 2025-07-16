#include "access_expression.hpp"
#include "type.hpp"

namespace cmp {

std::vector<uint8_t> AccessExpression::generate_bytecode(BytecodeGenerationInfo& bgi) const {
    std::vector<uint8_t> result;

    std::vector<uint8_t> result_left = left->generate_bytecode(bgi);
    result.insert(result.end(), result_left.begin(), result_left.end());

    std::vector<uint8_t> result_right = right->generate_bytecode(bgi);
    result.insert(result.end(), result_right.begin(), result_right.end());

    return result;
}

size_t AccessExpression::get_bytecode_size(BytecodeGenerationInfo &bgi) const {
    return left->get_bytecode_size(bgi) + right->get_bytecode_size(bgi);
}

std::string AccessExpression::to_string() const {
    return "(" + left->to_string() + "." + right->to_string() + ")";
}

std::vector<ExprPtr*> AccessExpression::get_expressions() {
    return {&left, &right};
}

bool AccessExpression::is_unresolved_symbol() const {
    return left->is_unresolved_symbol() || right->is_unresolved_symbol();
}

void AccessExpression::resolve(NameAnalysisInfo& name_analysis_info) {
    left->resolve(name_analysis_info);
    right->resolve(name_analysis_info);
}

TypePtr AccessExpression::get_type() const {
    return right->get_type();
}

}