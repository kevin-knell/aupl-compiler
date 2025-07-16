#pragma once
#include "expression.hpp"

namespace cmp
{

struct AccessExpression : public Expression {
    ExprPtr left;
    ExprPtr right;

    AccessExpression(ExprPtr left, ExprPtr right)
        : left(std::move(left)), right(std::move(right)) {}

    std::string to_string() const override;
    std::vector<uint8_t> generate_bytecode(BytecodeGenerationInfo& bgi) const override;
    size_t get_bytecode_size(BytecodeGenerationInfo& bgi) const override;
    std::vector<ExprPtr*> get_expressions() override;
    bool is_unresolved_symbol() const override;
    void resolve(NameAnalysisInfo& name_analysis_info) override;
    TypePtr get_type() const override;
    KIND get_kind() const override { return ACCESS; }
};

} // namespace cmp
