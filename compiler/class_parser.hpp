#pragma once
#include <string>
#include "source_file.hpp"

namespace cmp
{

class SymbolTable;

void parse_file(const std::string& file_path, const std::string& file_content, SymbolTable& symbol_table);
	
} // namespace cmp




