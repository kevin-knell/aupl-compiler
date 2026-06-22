#include "symbol_table.hpp"
#include "static_class_type.hpp"
#include "class_db.hpp"
#include "type_from_cpp.hpp"
#include "source_location.hpp"

#include <format>

namespace cmp {

SymbolTable::SymbolTable(vm::ClassDB &db) : class_db(db) {
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

	for (auto& cls_1 : db.classes) {
		std::shared_ptr<ClassType> nat_1 = native_types[cls_1.name];
		const vm::ClassBind* class_bind_1 = nat_1->class_bind;

		if (class_bind_1->id == class_bind_1->parent_id) {
			continue;
		}

		for (auto& cls_2 : db.classes) {
			std::shared_ptr<ClassType> nat_2 = native_types[cls_2.name];
			const vm::ClassBind* class_bind_2 = nat_2->class_bind;
			
			if (class_bind_1->parent_id == class_bind_2->id) {
				nat_1->class_ptr->parent = nat_2->class_ptr;
				nat_1->class_ptr->scope->upper_scope = nat_2->class_ptr->scope;
				break;
			}
		}
	}

	for (vm::ClassBind& cls : db.classes) {
		std::shared_ptr<ClassType> nat = native_types[cls.name];
		auto class_symbol = nat->class_ptr;

		//std::cout << "native class: " << nat->to_string() << std::endl;

		for (vm::VariableBind& v : cls.variables) {
			auto nat_var = VariableSymbol::create(v);
			COMPILER_ASSERT(nat_var->type->get_kind() != Type::INVALID, nat_var->to_string() + " from class " + cls.name);
			
			nat->class_ptr->scope->add_variable(nat_var);
			//std::cout << "\tnative var: " << nat_var->to_string() << std::endl;
		}
		
		for (vm::MethodPair& f : cls.methods) {
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
		Error::Level level) const {
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

	if (start_token_idx == end_token_idx) {
		end_token_idx = start_token_idx + 1;
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

	COMPILER_ASSERT(start_token_idx < end_token_idx, "start token must come before end token");
	
	return error;
}

}
