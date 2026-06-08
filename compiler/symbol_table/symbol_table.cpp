#include "symbol_table.hpp"
#include "static_class_type.hpp"
#include "class_db.hpp"
#include "type_from_cpp.hpp"
#include "source_location.hpp"

#include <format>

namespace cmp {

SymbolTable::SymbolTable(vm::ClassDB &db) {
	global_scope = Scope::create(Scope::SCOPE_TYPE::GLOBAL, "global");

	// TODO: add global scope

	for (auto& cls : db.classes) {
		auto class_symbol = ClassSymbol::create(cls);
		class_symbol->scope = Scope::create(Scope::CLASS, cls.name);
		
		std::shared_ptr<ClassType> nat = class_symbol->type;
		native_types[cls.name] = nat;
		nat->class_ptr = class_symbol;

		named_cpp_types()[cls.name] = nat;
	}

	for (auto& cls : db.classes) {
		std::shared_ptr<ClassType> nat = native_types[cls.name];
		auto class_symbol = nat->class_ptr;

		//std::cout << "native class: " << nat->to_string() << std::endl;

		for (auto& v : cls.variables) {
			auto nat_var = VariableSymbol::create(v);
			nat->class_ptr->scope->variables[v.name] = nat_var;
			//std::cout << "\tnative var: " << nat_var->to_string() << std::endl;
		}
		
		for (auto& f : cls.methods) {
			auto nat_func = FunctionSymbol::create(f);
			class_symbol->functions[f.name] = nat_func;
			
			if (f.is_global) {
				global_native_functions.push_back(nat_func);
			}
		}

		classes[cls.name] = class_symbol;
	}
}

void SymbolTable::generate_scope_structures() const {
	for (auto [n, cls] : classes) {
		if (cls->native_class_bind) continue;
        cls->static_scope->generate_structure();
    }

    for (auto [n, cls] : classes) {
		if (cls->native_class_bind) continue;
        cls->scope->generate_structure();
    }

    for (auto [n, cls] : classes) {
		if (cls->native_class_bind) continue;
        for (auto [fn, f] : cls->functions) {
            f->scope->generate_structure();
        }
    }
}

Error &SymbolTable::add_error(
		const SourceLocation& source_location,
		const std::string message,
		Error::Level level) {
	const SourceFile* source_file = source_location.source_file;
	
	std::string path = "(unknown file)";
	size_t start_token_idx = 0; 
	size_t end_token_idx = 0; 
	size_t line = 0;
	size_t col = 0;

	if (source_file) {
		path = source_file->path;
		start_token_idx = source_location.start_token_index;
		end_token_idx = source_location.end_token_index;
		line = source_file->tokens[start_token_idx].line;
		col = source_file->tokens[start_token_idx].col;
	}

	Error& error = errors.emplace_back(
		path,
		start_token_idx,
		end_token_idx,
		line,
		col,
		message,
		level
	);

	std::cerr <<
			path << ":" << line << ":" << col << ": " <<
			error.get_error_text() <<
			message << std::endl;

	std::cerr << source_location.get_text_as_rect() << "\n" << std::endl;
	
	return error;
}

}
