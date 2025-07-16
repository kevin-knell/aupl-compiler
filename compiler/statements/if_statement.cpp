#include "if_statement.hpp"
#include "expression.hpp"

#include "color.hpp"

namespace cmp {

IfStatement::IfStatement(ExprPtr condition,
                         StmtPtr if_block,
                         StmtPtr else_block)
    : condition(std::move(condition)),
      if_block(std::dynamic_pointer_cast<BlockStatement>(if_block)),
      else_block(std::dynamic_pointer_cast<BlockStatement>(else_block)) {}

std::vector<uint8_t> IfStatement::generate_bytecode(BytecodeGenerationInfo&) const {
    return {}; // Implement your bytecode logic here
}

size_t IfStatement::get_bytecode_size(BytecodeGenerationInfo&) const {
    return 0; // Implement your size logic here
}

Statement::KIND IfStatement::get_kind() const {
    return IF;
}

std::vector<ExprPtr*> IfStatement::get_expressions() {
    return { &condition };
}

std::string IfStatement::to_string() const {
    auto result = C_KEYWORD("if ") + condition->to_string() + " ";
    
    result += if_block->to_string();

    return result;
}

} // namespace cmp
