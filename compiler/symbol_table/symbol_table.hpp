#pragma once

#include "class_symbol.hpp"
#include <unordered_map>
#include <string>
#include "class_db.hpp"
#include "forward_declarations.hpp"

namespace vm {
	class ClassDB;
}

namespace cmp {

struct SymbolTable {
    std::unordered_map<std::string, ClassPtr> classes;
	std::unordered_map<std::string, TypePtr> native_types;
	std::vector<FuncPtr> global_native_functions;
	std::vector<vm::Value> const_memory;

	SymbolTable(vm::ClassDB& db);
};

}