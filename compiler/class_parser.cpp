#include "class_parser.hpp"
#include "symbol_table.hpp"
#include "tokenizer.hpp"
#include "symbol_builder.hpp"

namespace cmp {

void parse_file(const std::string& file_path, const std::string &file_content, SymbolTable &symbol_table) {
	auto tokens = tokenize(file_content);

	symbol_table.source_files.emplace(file_path, SourceFile(file_path, file_content, tokens));

	SourceFile& source_file = symbol_table.source_files.at(file_path);
	
	SymbolBuilder builder(source_file, symbol_table);
	builder.parse_class();
}

}