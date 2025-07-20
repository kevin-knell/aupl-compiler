#include "label_statement.hpp"

#include <iostream>

namespace cmp {

std::vector<uint8_t> LabelStatement::generate_bytecode(BytecodeGenerationInfo &bgi) const {
    return {};
}

size_t LabelStatement::get_bytecode_size(BytecodeGenerationInfo &bgi) const {
	bgi.scope->label_addresses[std::make_shared<LabelStatement>(*this)] = bgi.bytecode_size;
	std::cout << "Label: " << identifier << " at address: " << bgi.bytecode_size << std::endl;
    return size_t();
}

Statement::KIND LabelStatement::get_kind() const
{
    return KIND();
}
std::vector<ExprPtr *> LabelStatement::get_expressions()
{
    return std::vector<ExprPtr *>();
}
std::string LabelStatement::to_string() const
{
    return "@" + identifier;
}
}