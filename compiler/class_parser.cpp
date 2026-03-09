#include "class_parser.hpp"
#include "symbol_table.hpp"
#include "tokenizer.hpp"
#include "symbol_builder.hpp"

void cmp::parse_file(const std::string &file_content, SymbolTable &symbol_table) {
	auto tokens = tokenize(file_content);
	
	SymbolBuilder builder(tokens, symbol_table);
	builder.parse_class();
}