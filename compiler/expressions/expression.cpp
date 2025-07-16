#include "expression.hpp"
#include "variable_symbol.hpp"
#include "variable_expression.hpp"
#include "declare_statement.hpp"
#include <assert.h>

namespace cmp {

std::vector<ExprPtr*> Expression::get_expressions() {
    return {};
}

int Expression::get_level() {
    int max = 0;
    auto expr = get_expressions();
    for (auto p : expr)
        max = std::max((*p)->get_level() + 1, max);
    return max;
}

}