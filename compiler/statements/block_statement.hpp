#pragma once
#include "statement.hpp"

namespace cmp
{

struct BlockStatement : Statement {
    ScopePtr scope;

    BlockStatement(ScopePtr scope) : scope(scope) {}

    std::vector<uint8_t> generate_bytecode(BytecodeGenerationInfo&) const override;
    size_t get_bytecode_size(BytecodeGenerationInfo&) const override;

    KIND get_kind() const override;

    std::vector<ExprPtr*> get_expressions() override;

    std::string to_string() const override;
};

} // namespace cmp
