#include "variable_expression.hpp"
#include "scope.hpp"
#include "instructions.hpp"
#include "symbol_table.hpp"
#include "variable_symbol.hpp"
#include "invalid_type.hpp"
#include "text_color.hpp"
#include "shared_type.hpp"
#include "variable_expression.hpp"
#include "compiler_error.hpp"

namespace cmp {
std::vector<ExprPtr *> VariableExpression::get_expressions() {
	return obj_expr ? std::vector({&obj_expr}) : std::vector<ExprPtr *>();
}

std::string VariableExpression::to_string() const {
	std::string obj_string = obj_expr ? obj_expr->to_string() + "." : "";
    std::string name_string = is_unresolved_symbol() == true ? C_UNRES(name) : var->name_to_string();
	return obj_string + name_string;
}

bool VariableExpression::is_unresolved_symbol() const {
    return !var
		|| !var->type
		|| var->type->get_kind() == Type::INVALID;
}

TypePtr VariableExpression::get_type() const {
    return var ? var->type : InvalidType::make("unknown variable: " + name);
}

bool VariableExpression::is_constexpr() const {
    if (!var->initial_value) return false;
    else return var->initial_value->is_constexpr();
}

vm::Value* VariableExpression::eval_constexpr() const {
    if (!var->initial_value) return nullptr;
    else return var->initial_value->eval_constexpr();
}

}