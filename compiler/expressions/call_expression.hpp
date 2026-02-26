// call_expression.hpp
#pragma once
#include "expression.hpp"

namespace cmp {

struct CallExpression : public Expression {
    std::string name;
    std::vector<ExprPtr> arguments;
	ExprPtr obj_expr;
    FuncPtr f;

    CallExpression(std::string name, const std::vector<ExprPtr>& arguments, ExprPtr obj_expr)
        : name(name), arguments(arguments), obj_expr(obj_expr) {}

    std::string to_string() const override;
    std::vector<uint8_t> generate_bytecode(BytecodeGenerationInfo& bgi) const override;
    size_t get_bytecode_size(BytecodeGenerationInfo& bgi) const override;
    TypePtr get_type() const override;
    bool is_unresolved_symbol() const override;
    void resolve(NameAnalysisInfo& name_analysis_info) override;
    std::vector<ExprPtr*> get_expressions() override;
    bool is_pure() const override;
    KIND get_kind() const override { return CALL; }
};

}