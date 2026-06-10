#pragma once

#include "class_symbol.hpp"
#include <unordered_map>
#include <string>
#include "class_db.hpp"
#include "forward_declarations.hpp"
#include "source_file.hpp"

namespace vm {
	class ClassDB;
}

namespace cmp {

struct SymbolTable {
    std::unordered_map<std::string, ClassPtr> classes;
    std::vector<Error> errors;
	std::unordered_map<std::string, std::shared_ptr<ClassType>> native_types;
	FuncVec global_native_functions;
	std::vector<vm::Value> const_memory;
	std::map<std::string, SourceFile> source_files;
	ScopePtr global_scope;
	vm::ClassDB& class_db;

	SymbolTable(vm::ClassDB& db);

	SymbolTable(const SymbolTable& other) = delete;
	SymbolTable& operator= (const SymbolTable& other) = delete;

	void generate_scope_structures() const;

	Error& add_error(
		const SourceLocation& source_location,
		const std::string message,
		Error::Level level);
};

}