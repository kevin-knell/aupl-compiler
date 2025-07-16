#pragma once
#include "statement.hpp"
#include "expression.hpp"
#include "type.hpp"

namespace cmp {

struct DeclareStatement : public Statement {
    VarPtr variable_symbol;

    DeclareStatement(const VarPtr& variable_symbol);

    std::string to_string() const override;

    std::vector<uint8_t> generate_bytecode(BytecodeGenerationInfo& bgi) const override;
    virtual size_t get_bytecode_size(BytecodeGenerationInfo& bgi) const override;

    std::vector<ExprPtr*> get_expressions() override;

    virtual KIND get_kind() const override { return DECLARE; }
};

} // namespace cmp
