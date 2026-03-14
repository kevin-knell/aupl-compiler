#include "declare_statement.hpp"
#include "instructions.hpp"
#include "primitive_type.hpp"
#include "value.hpp"
#include "binary_op_expression.hpp"
#include "variable_expression.hpp"
#include "load_const_expression.hpp"
#include "color.hpp"
#include "call_expression.hpp"
#include "class_db.hpp"
#include "string_literal_expression.hpp"
#include "symbol_table.hpp"
#include "bytecode_generator.hpp"

#define TAG(s, b) std::string(b ? s : "")

namespace cmp {

DeclareStatement::DeclareStatement(const VarPtr& variable_symbol)
    : variable_symbol(variable_symbol) {}

std::string DeclareStatement::to_string() const {
    return TAG(C_KEYWORD("volatile "), is_volatile) + variable_symbol->to_string() + " " +
           (variable_symbol->initial_value
               ? "= " + variable_symbol->initial_value->to_string()
               : "");
}

std::vector<ExprPtr*> DeclareStatement::get_expressions() {
    if (variable_symbol->initial_value) return {&variable_symbol->initial_value};
    else return {};
}

} // namespace cmp

#undef TAG