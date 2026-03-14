#include "label_statement.hpp"
#include "bytecode_generator.hpp"

#include <iostream>

namespace cmp {

Statement::KIND LabelStatement::get_kind() const {
    return LABEL;
}

std::vector<ExprPtr *> LabelStatement::get_expressions() {
    return std::vector<ExprPtr *>();
}

std::string LabelStatement::to_string() const {
    return "@" + identifier;
}

}