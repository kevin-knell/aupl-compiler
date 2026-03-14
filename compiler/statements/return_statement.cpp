#include "return_statement.hpp"

namespace cmp {

std::string ReturnStatement::to_string() const {
    return "return " + (return_value ? return_value->to_string() : "");
}

std::vector<ExprPtr*> ReturnStatement::get_expressions() {
    if (return_value) return {&return_value};
	else return {};
}

}