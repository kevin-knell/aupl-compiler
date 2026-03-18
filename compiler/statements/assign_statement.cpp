#include "assign_statement.hpp"
#include "instructions.hpp"
#include "primitive_type.hpp"
#include "variable_expression.hpp"
#include "binary_op_expression.hpp"
#include "bytecode_generator.hpp"
#include <assert.h>

namespace cmp {

AssignmentStatement::AssignmentStatement(ExprPtr expr_left, ExprPtr expr_right)
	: expr_left(std::move(expr_left)), expr_right(std::move(expr_right)) {}

std::string AssignmentStatement::to_string() const {
	return expr_left->to_string() + " = " + expr_right->to_string();
}

std::vector<ExprPtr*> AssignmentStatement::get_expressions() {
	return {&expr_right};
}

} // namespace cmp
