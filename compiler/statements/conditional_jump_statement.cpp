#include "conditional_jump_statement.hpp"
#include "variable_expression.hpp"
#include "label.hpp"
#include "binary_op_expression.hpp"
#include "primitive_type.hpp"
#include "bytecode_generator.hpp"

namespace cmp {

std::string ConditionalJumpStatement::to_string() const {
    if (!condition) {
		return "goto " + if_label->to_string();
	}

	return "goto " +
		condition->to_string() + " ? " +
		if_label->to_string() +
		(else_label ? " : " + else_label->to_string() : "");
}

} // namespace cmp
