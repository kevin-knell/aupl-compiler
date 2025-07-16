// expression.hpp
#pragma once
#include <memory>
#include <string>
#include <vector>
#include <cstdint>
#include "instructions.hpp"
#include "name_analysis_info.hpp"
#include "bytecode_generation_info.hpp"
#include "forward_declarations.hpp"
#include <value.hpp>

namespace cmp {

struct Expression {
    enum KIND {
        LOAD_CONST,

        INT_LIT,
        DOUBLE_LIT,
        STRING_LIT,

        VARIABLE,
        CALL,

        ACCESS,

        UNARY,
        BINARY,
        TERNARY
    };

    virtual ~Expression() = default;

    virtual std::string to_string() const = 0;
    
    virtual std::vector<uint8_t> generate_bytecode(BytecodeGenerationInfo& bgi) const { return { (uint8_t)vm::Instruction::ERR }; };
    virtual size_t get_bytecode_size(BytecodeGenerationInfo& bgi) const { return 1; };
    
    virtual std::vector<ExprPtr*> get_expressions();
    virtual int get_level();
    
    virtual bool is_unresolved_symbol() const { return false; }
    virtual void resolve(NameAnalysisInfo& name_analysis_info) {
        (void)name_analysis_info;
    };
    
    virtual TypePtr get_type() const = 0;

    virtual bool is_constexpr() const { return false; };
    virtual vm::Value* eval_constexpr() const { return nullptr; };

    virtual bool is_pure() const { return is_constexpr(); };
    virtual vm::Value* eval_pure() const { return is_constexpr() ? eval_constexpr() : nullptr; };

    virtual KIND get_kind() const = 0;
};
using ExprPtr = std::shared_ptr<Expression>;

}