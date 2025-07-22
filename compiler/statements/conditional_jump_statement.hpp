#pragma once
#include "statement.hpp"
#include "expression.hpp"
#include <string>

namespace cmp {

class Label;
using LabelPtr = std::shared_ptr<Label>;

class ConditionalJumpStatement : public Statement {
public:
    ExprPtr condition;
    LabelPtr if_label;
    LabelPtr else_label;

    ConditionalJumpStatement(ExprPtr cond, LabelPtr if_lbl, LabelPtr else_lbl)
        : condition(cond), if_label(if_lbl), else_label(else_lbl) {};

    std::vector<uint8_t> generate_bytecode(BytecodeGenerationInfo& bgi) const override;
    size_t get_bytecode_size(BytecodeGenerationInfo& bgi) const override;
    KIND get_kind() const override { return KIND::IF; } // Or add JUMP if you want a new kind
    std::string to_string() const override;
    std::vector<ExprPtr*> get_expressions() override {
        return { &condition };
    }
};

} // namespace cmp
