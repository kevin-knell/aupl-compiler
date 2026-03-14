#include "access_expression.hpp"
#include "type.hpp"

namespace cmp {

std::string AccessExpression::to_string() const {
    return "(" + left->to_string() + "." + right->to_string() + ")";
}

std::vector<ExprPtr*> AccessExpression::get_expressions() {
    return { &right };
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