#pragma once

#include "statement.hpp"
#include "block_statement.hpp"

namespace cmp {

struct IfStatement : public Statement {
    ExprPtr condition;
    std::shared_ptr<BlockStatement> if_block;
    std::shared_ptr<BlockStatement> else_block;

    IfStatement(ExprPtr condition,
                StmtPtr if_block,
                StmtPtr else_block);

    std::vector<uint8_t> generate_bytecode(BytecodeGenerationInfo&) const override;
    size_t get_bytecode_size(BytecodeGenerationInfo&) const override;

    KIND get_kind() const override;

    std::vector<ExprPtr*> get_expressions() override;

    std::string to_string() const override;
};

} // namespace cmp
