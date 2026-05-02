#pragma once
#include "symbol_table.hpp"

namespace cmp
{
    
class NameAnalyzer {
public:
	static bool na_debug_print;
	static bool na_debug_print_verbose;

    SymbolTable& symbol_table;

    NameAnalyzer(SymbolTable& symbol_table) : symbol_table(symbol_table) {}

    void resolve_variables() const;
};

} // namespace cmp
