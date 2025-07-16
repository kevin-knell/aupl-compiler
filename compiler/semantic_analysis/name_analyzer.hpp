#pragma once
#include "symbol_table.hpp"

namespace cmp
{
    
class NameAnalyzer {
public:
    SymbolTable& symbol_table;

    NameAnalyzer(SymbolTable& symbol_table) : symbol_table(symbol_table) {}

    void resolve_variables() const;
};

} // namespace cmp
