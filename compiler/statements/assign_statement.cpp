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

/*
namespace {

    std::vector<uint8_t> generate_copy_to_static_bytecode(
			const uint16_t offset_static,
			const uint16_t offset_local,
			const int size,
			BytecodeGenerationInfo& bgi) {
        std::vector<uint8_t> result;
        if (size == 0) return {};
		
        vm::Value2 dest{ .u16 = offset_static };
        vm::Value2 local{ .u16 = offset_local };
        switch (size) {
            case 1: result.push_back((uint8_t)vm::Instruction::COPY_TO_STATIC_1); break;
            case 2: result.push_back((uint8_t)vm::Instruction::COPY_TO_STATIC_2); break;
            case 4: result.push_back((uint8_t)vm::Instruction::COPY_TO_STATIC_4); break;
            case 8: result.push_back((uint8_t)vm::Instruction::COPY_TO_STATIC_8); break;
            case 16: result.push_back((uint8_t)vm::Instruction::COPY_TO_STATIC_16); break;
            default:
                result.push_back((uint8_t)vm::Instruction::COPY_TO_STATIC_VAR);
                result.push_back(size);
        }
        result.push_back(dest.v[0].u8);
        result.push_back(dest.v[1].u8);
        result.push_back(local.v[0].u8);
        result.push_back(local.v[1].u8);
        return result;
    }
}
*/

std::vector<ExprPtr*> AssignmentStatement::get_expressions() {
	return {&expr_right};
}

} // namespace cmp
