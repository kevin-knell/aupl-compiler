#include "label_statement.hpp"

namespace cmp {

std::vector<uint8_t> LabelStatement::generate_bytecode(BytecodeGenerationInfo &bgi) const {
    return {};
}

size_t LabelStatement::get_bytecode_size(BytecodeGenerationInfo &bgi) const
{
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