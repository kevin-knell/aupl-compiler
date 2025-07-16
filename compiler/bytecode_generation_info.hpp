#pragma once
#include <memory>
#include "forward_declarations.hpp"

namespace cmp {

class SymbolTable;

class ClassSymbol;
using ClassPtr = std::shared_ptr<ClassSymbol>;

class FunctionSymbol;
using FuncPtr = std::shared_ptr<FunctionSymbol>;

class Scope;
using ScopePtr = std::shared_ptr<Scope>;

struct BytecodeGenerationInfo {
    SymbolTable& symbol_table;
    ClassPtr cls;
    FuncPtr f;
    ScopePtr scope;
    int label_number = 0;

    BytecodeGenerationInfo(SymbolTable& symbol_table, ClassPtr cls, FuncPtr f, ScopePtr scope)
			: symbol_table(symbol_table), cls(cls), f(f), scope(scope) {}
};

} // namespace cmp
