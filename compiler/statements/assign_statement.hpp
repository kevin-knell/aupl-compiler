#pragma once
#include "statement.hpp"
#include "expression.hpp"

namespace cmp {

struct AssignmentStatement : public Statement {
    ExprPtr expr_left;
    ExprPtr expr_right;

    AssignmentStatement(ExprPtr expr_left, ExprPtr expr_right);

    std::string to_string() const override;

    std::vector<uint8_t> generate_bytecode(BytecodeGenerationInfo& bgi) const override;
    virtual size_t get_bytecode_size(BytecodeGenerationInfo& bgi) const override;

    std::vector<ExprPtr*> get_expressions() override;

    virtual KIND get_kind() const override { return ASSIGN; }
};

} // namespace cmp
