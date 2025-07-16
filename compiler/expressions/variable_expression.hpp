#pragma once
#include "expression.hpp"
#include "variable_symbol.hpp"

namespace cmp {

struct VariableExpression : public Expression {
    VarPtr var;
    std::string name;

    VariableExpression(const std::string& name) : name(name) {}
    VariableExpression(VarPtr var) : var(var), name(var->name) {}

    std::string to_string() const override;
    std::vector<uint8_t> generate_bytecode(BytecodeGenerationInfo& bgi) const override;
    size_t get_bytecode_size(BytecodeGenerationInfo& bgi) const override;
    bool is_unresolved_symbol() const override;
    void resolve(NameAnalysisInfo& name_analysis_info) override;
    TypePtr get_type() const override;
    bool is_constexpr() const override;
    vm::Value* eval_constexpr() const override;
    KIND get_kind() const override { return VARIABLE; }
};

using VarExprPtr = std::shared_ptr<VariableExpression>;

}