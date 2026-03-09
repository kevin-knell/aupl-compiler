#include "conditional_jump_statement.hpp"
#include "variable_expression.hpp"
#include "label.hpp"
#include "binary_op_expression.hpp"
#include "primitive_type.hpp"
#include "bytecode_generator.hpp"

namespace cmp {

std::vector<uint8_t> generate_variable_bytecode(
		const VarPtr& variable,
		const LabelPtr& if_label,
		const LabelPtr& else_label,
		BytecodeGenerationInfo& bgi) {
	vm::Value2 src{ .u16 = static_cast<uint16_t>(Scope::get_variable_index(bgi.scope, variable->name)) };
	vm::Value4 if_addr{ .u32 = if_label->get_address() };

	std::vector<uint8_t> result;

	result.push_back(static_cast<uint8_t>(vm::Instruction::IF_NEQ_CONST_I8));
	result.push_back(src.v[0].u8);
	result.push_back(src.v[1].u8);
	result.push_back(0);
	result.push_back(if_addr.v[0].u8);
	result.push_back(if_addr.v[1].u8);
	result.push_back(if_addr.v[2].u8);
	result.push_back(if_addr.v[3].u8);

	if (else_label) {
		vm::Value4 else_addr{ .u32 = else_label->get_address() };
		result.push_back(static_cast<uint8_t>(vm::Instruction::GOTO));
		result.push_back(else_addr.v[0].u8);
		result.push_back(else_addr.v[1].u8);
		result.push_back(else_addr.v[2].u8);
		result.push_back(else_addr.v[3].u8);
	}

	return result;
}

std::vector<uint8_t> generate_binary_bytecode(
		const std::shared_ptr<BinaryExpression>& binary_expression,
		const LabelPtr& if_label,
		const LabelPtr& else_label,
		BytecodeGenerationInfo& bgi) {
	auto left_var = std::dynamic_pointer_cast<VariableExpression>(binary_expression->left)->var;
	auto right_var = std::dynamic_pointer_cast<VariableExpression>(binary_expression->right)->var;
	vm::Value2 left{ .u16 = static_cast<uint16_t>(Scope::get_variable_index(bgi.scope, left_var->name)) };
	vm::Value2 right{ .u16 = static_cast<uint16_t>(Scope::get_variable_index(bgi.scope, right_var->name)) };
	vm::Value4 if_addr{ .u32 = if_label->get_address() };

	std::vector<uint8_t> result;

	auto bin_type = std::dynamic_pointer_cast<PrimitiveType>(left_var->type)->vm_bin_type;

	auto instruction = vm::get_binary_opcode(bin_type, binary_expression->op, false);

	std::cout << left_var->name << " ? " << right_var->name << std::endl;

	result.push_back(static_cast<uint8_t>(instruction));
	result.push_back(left.v[0].u8);
	result.push_back(left.v[1].u8);
	result.push_back(right.v[0].u8);
	result.push_back(right.v[1].u8);
	result.push_back(if_addr.v[0].u8);
	result.push_back(if_addr.v[1].u8);
	result.push_back(if_addr.v[2].u8);
	result.push_back(if_addr.v[3].u8);

	if (else_label) {
		vm::Value4 else_addr{ .u32 = else_label->get_address() };
		result.push_back(static_cast<uint8_t>(vm::Instruction::GOTO));
		result.push_back(else_addr.v[0].u8);
		result.push_back(else_addr.v[1].u8);
		result.push_back(else_addr.v[2].u8);
		result.push_back(else_addr.v[3].u8);
	}

	return result;
}

std::vector<uint8_t> ConditionalJumpStatement::generate_bytecode(BytecodeGenerationInfo& bgi) const {
	if (!condition) {
		vm::Value4 addr{ .u32 = if_label->get_address() };

		std::vector<uint8_t> result;
		result.push_back(static_cast<uint8_t>(vm::Instruction::GOTO));
		result.push_back(addr.v[0].u8);
		result.push_back(addr.v[1].u8);
		result.push_back(addr.v[2].u8);
		result.push_back(addr.v[3].u8);

		return result;
	}

	switch(condition->get_kind()) {
		case Expression::VARIABLE:
			return generate_variable_bytecode(
				std::dynamic_pointer_cast<VariableExpression>(condition)->var,
				if_label,
				else_label,
				bgi);
		case Expression::BINARY:
			return generate_binary_bytecode(
				std::dynamic_pointer_cast<BinaryExpression>(condition),
				if_label,
				else_label,
				bgi);
		default:
				return { (uint8_t)vm::Instruction::ERR };
	}
}

size_t ConditionalJumpStatement::get_bytecode_size(BytecodeGenerationInfo& bgi) const {
    if (!condition) return 1 + 4;

	switch(condition->get_kind()) {
		case Expression::VARIABLE: return (1 + 2 + 1 + 4) + (else_label ? 1 + 4 : 0);
		case Expression::BINARY: return (1 + 2 + 2 + 4) + (else_label ? 1 + 4 : 0);
		default: return 1;
	}
}

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
