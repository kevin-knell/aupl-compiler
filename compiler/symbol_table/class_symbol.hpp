#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>
#include "variable_symbol.hpp"
#include "function_symbol.hpp"
#include "error.hpp"

namespace cmp {

struct ClassSymbol {
    std::string name;
    std::shared_ptr<ClassSymbol> parent;
    ScopePtr static_scope;
    ScopePtr scope;
    std::map<std::string, FuncPtr> functions;
    std::vector<Error> errors;
    VarPtr static_var;
    TypePtr type;
    bool is_declared;

    ClassSymbol()
        : name(""), parent(nullptr), is_declared(false) {}

    ClassSymbol(const std::string& name) : name(name) {}
};

using ClassPtr = std::shared_ptr<ClassSymbol>;

}