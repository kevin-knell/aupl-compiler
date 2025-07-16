#pragma once
#include "type.hpp"
//#include "expression.hpp"
#include <string>
#include "forward_declarations.hpp"

namespace cmp {

struct VariableSymbol {
    enum class MemoryType {
        VALUE,      // value directly in memory
        SHARED,     // shared_ptr
        MANUAL,     // new, delete
        STATIC      // when an object exists exactly once (?)
    };

    bool is_public = false;
    bool is_static = false;
    bool is_const = false;
    TypePtr type;
    std::string name;
    ExprPtr initial_value;
    ScopePtr scope;

    VariableSymbol() {};

    VariableSymbol(TypePtr type, const std::string& name, ExprPtr initial_value)
        : type(std::move(type)), name(name), initial_value(std::move(initial_value)) {}
    
    std::string to_string() const;
    std::string name_to_string() const;
};

using VarPtr = std::shared_ptr<VariableSymbol>;

}