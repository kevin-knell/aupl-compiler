#pragma once
#include <cstddef>
#include "forward_declarations.hpp"

namespace cmp
{

struct BytecodeGenerationInfo {
	SymbolTable& symbol_table;
	ClassPtr cls;
	FuncPtr f;
	ScopePtr scope;
	size_t bytecode_size = 0;

	BytecodeGenerationInfo(SymbolTable& symbol_table, ClassPtr cls, FuncPtr f, ScopePtr scope)
			: symbol_table(symbol_table), cls(cls), f(f), scope(scope) {}
};

struct BytecodeProductInfo {
	std::vector<uint8_t> bytecode;
    size_t main_start;
    bool has_main = false;
};

size_t generate_bytecode_layout(SymbolTable& symbol_table);
BytecodeProductInfo generate_bytecode(SymbolTable& symbol_table, size_t bytecode_size);

} // namespace cmp
