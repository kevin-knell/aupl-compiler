#pragma once
#include "statement.hpp"

namespace cmp
{

struct LabelStatement : Statement {
    std::string identifier;

	LabelStatement(const std::string& id) : identifier(id) {}

    std::vector<uint8_t> generate_bytecode(BytecodeGenerationInfo& bgi) const override;
    size_t get_bytecode_size(BytecodeGenerationInfo& bgi) const override;

    KIND get_kind() const override;

    std::vector<ExprPtr*> get_expressions() override;

    std::string to_string() const override;
};

} // namespace cmp
