#pragma once
#include <string>

namespace cmp
{

class SymbolTable;

void parse_file(const std::string& file_content, SymbolTable& symbol_table);
	
} // namespace cmp




