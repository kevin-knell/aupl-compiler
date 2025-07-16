// statement.hpp
#pragma once
#include <memory>
#include <string>
#include <vector>
#include "function_symbol.hpp"
#include "bytecode_generation_info.hpp"
#include "forward_declarations.hpp"



namespace cmp {

struct Statement {
    enum KIND {
        DECLARE,
        ASSIGN,
        EXPRESSION,
        BLOCK,
        IF,
        RETURN,
        LABEL
    };

	bool is_volatile = false;

    virtual ~Statement() = default;

    virtual std::vector<uint8_t> generate_bytecode(BytecodeGenerationInfo& bgi) const = 0;
    virtual size_t get_bytecode_size(BytecodeGenerationInfo& bgi) const = 0;

    virtual std::vector<ExprPtr*> get_expressions() { return {}; };

    virtual KIND get_kind() const = 0;

    virtual std::string to_string() const = 0;
};

using StmtPtr = std::shared_ptr<Statement>;

}