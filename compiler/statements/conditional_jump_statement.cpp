#include "conditional_jump_statement.hpp"
#include "variable_expression.hpp"
#include "label.hpp"

namespace cmp {

std::vector<uint8_t> generate_variable_bytecode(
		const VarPtr& variable,
		const LabelPtr& if_label,
		const LabelPtr& else_label,
		BytecodeGenerationInfo& bgi) {
	vm::Value2 src{ .u16 = static_cast<uint16_t>(bgi.scope->variable_indices[variable->name]) };
	vm::Value4 if_addr{ .u32 = if_label->scope->starting_address + if_label->scope->label_addresses[if_label->label_stmt] };
	vm::Value4 else_addr{ .u32 = else_label->scope->starting_address + else_label->scope->label_addresses[else_label->label_stmt] };

	std::vector<uint8_t> result;

	result.push_back(static_cast<uint8_t>(vm::Instruction::IF_NEQ_CONST_I8));
	result.push_back(src.v[0].u8);
	result.push_back(src.v[1].u8);
	result.push_back(0);
	result.push_back(if_addr.v[0].u8);
	result.push_back(if_addr.v[1].u8);
	result.push_back(if_addr.v[2].u8);
	result.push_back(if_addr.v[3].u8);

	result.push_back(static_cast<uint8_t>(vm::Instruction::GOTO));
	result.push_back(else_addr.v[0].u8);
	result.push_back(else_addr.v[1].u8);
	result.push_back(else_addr.v[2].u8);
	result.push_back(else_addr.v[3].u8);

	return result;
}

std::vector<uint8_t> ConditionalJumpStatement::generate_bytecode(BytecodeGenerationInfo& bgi) const {
	switch(condition->get_kind()) {
		case Expression::VARIABLE:
			return generate_variable_bytecode(
				std::dynamic_pointer_cast<VariableExpression>(condition)->var,
				if_label,
				else_label,
				bgi);
	}
    return {
		static_cast<uint8_t>(vm::Instruction::ERR)
	};
}

size_t ConditionalJumpStatement::get_bytecode_size(BytecodeGenerationInfo& bgi) const {
    // Size: condition + 4 bytes for 2 instructions and their label indices
    return 1 + 2 + 1 + 4 + 1 + 4;
}

std::string ConditionalJumpStatement::to_string() const {
    return "goto " +
		condition->to_string() + " ? " +
		if_label->to_string() +
		(else_label ? " : " + else_label->to_string() : "");
}

} // namespace cmp
