#include "block_statement.hpp"

#include "color.hpp"

namespace cmp {

Statement::KIND BlockStatement::get_kind() const {
    return BLOCK;
}

std::vector<ExprPtr *> BlockStatement::get_expressions() {
    return std::vector<ExprPtr *>();
}

std::string BlockStatement::to_string() const {
    std::string result = C_BRACE_L + "\n";

    for (auto s : scope->body) {
        result += s->to_string() + "\n";
    }

    result += "\n" + C_BRACE_R;
    return result;
}

}