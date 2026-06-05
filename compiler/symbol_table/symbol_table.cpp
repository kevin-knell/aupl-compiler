#include "symbol_table.hpp"
#include "native_class_type.hpp"
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
		
		std::shared_ptr<NativeClassType> nat = std::dynamic_pointer_cast<NativeClassType>(class_symbol->type);
		native_types[cls.name] = nat;
		nat->class_ptr = class_symbol;

		named_cpp_types()[cls.name] = nat;
	}

	for (auto& cls : db.classes) {
		std::shared_ptr<NativeClassType> nat = std::dynamic_pointer_cast<NativeClassType>(native_types[cls.name]);
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
			nat->functions.push_back(nat_func);
			//std::cout << "\tnative func: " << nat_func->to_string() << std::endl;
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
		const SourceFile& source_file,
		const size_t start_token_idx,
		const size_t end_token_idx,
		const std::string message,
		Error::Level level) {
	Token start_token = source_file.tokens[start_token_idx];
	Token end_token = source_file.tokens[end_token_idx];

	Error& error = errors.emplace_back(
		source_file.path,
		start_token_idx,
		end_token_idx,
		start_token.line,
		start_token.col,
		message,
		level
	);

	std::cerr <<
			source_file.path << ":" << start_token.line << ":" << start_token.col << ": " <<
			error.get_error_text() <<
			message << std::endl;

	size_t context_start = start_token.pos;
	size_t context_size = end_token.pos + end_token.value.size() - context_start;
	
	const std::string context = source_file.text.substr(context_start, context_size);

	SourceLocation source_location(source_file, start_token_idx, end_token_idx);

	std::cerr << source_location.get_text_as_rect();

	/*size_t current_line_in_context = start_token.line;
	std::stringstream context_with_lines;


	context_with_lines
		<< "\t"
		<< std::format("{:>4}| ", current_line_in_context);
	
	current_line_in_context += 1;

	for (auto it = context.begin(); it != context.end(); ++it) {
		char c = *it;

		context_with_lines << c;
		
		if (c == '\n') {
			context_with_lines
				<< "\t"
				<< std::format("{:>4}| ", current_line_in_context);
			
			current_line_in_context += 1;
		}
	}*/

	//std::cerr << context_with_lines.str() << std::endl;
	
	return error;
}

}
