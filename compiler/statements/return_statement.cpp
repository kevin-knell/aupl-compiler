#include "return_statement.hpp"

namespace cmp {

std::string ReturnStatement::to_string() const {
    return "return " + (return_value ? return_value->to_string() : "");
}

std::vector<ExprPtr*> ReturnStatement::get_expressions() {
    if (return_value) return {&return_value};
	else return {};
}

std::vector<uint8_t> ReturnStatement::generate_bytecode(BytecodeGenerationInfo& bgi) const {
    std::vector<uint8_t> result;

    //std::vector<uint8_t> result_value = return_value->generate_bytecode(bgi);
    //result.insert(result.end(), result_value.begin(), result_value.end());

    result.push_back((uint8_t)vm::Instruction::RET);

    return result;
}

size_t ReturnStatement::get_bytecode_size(BytecodeGenerationInfo& bgi) const {
    return 1;
}

}