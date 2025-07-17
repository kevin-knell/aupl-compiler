#include "conditional_jump_statement.hpp"
#include "label.hpp"

namespace cmp {

std::vector<uint8_t> ConditionalJumpStatement::generate_bytecode(BytecodeGenerationInfo& bgi) const {
    return {
		static_cast<uint8_t>(vm::Instruction::ERR)
	};
}

size_t ConditionalJumpStatement::get_bytecode_size(BytecodeGenerationInfo& bgi) const {
    // Size: condition + 4 bytes for 2 instructions and their label indices
    return 1;
}

std::string ConditionalJumpStatement::to_string() const {
    return "goto " +
		condition->to_string() + " ? " +
		if_label->to_string() +
		(else_label ? " : " + else_label->to_string() : "");
}

} // namespace cmp
