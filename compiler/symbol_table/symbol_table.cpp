#include "symbol_table.hpp"
#include "native_class_type.hpp"
#include "static_class_type.hpp"
#include "class_db.hpp"

namespace cmp {

SymbolTable::SymbolTable(vm::ClassDB &db) {
	global_scope = std::make_shared<Scope>(Scope::SCOPE_TYPE::GLOBAL, "global");

	// TODO: add global scope

	for (auto& cls : db.classes) {
		auto class_symbol = std::make_shared<ClassSymbol>(cls);
		class_symbol->scope = std::make_shared<Scope>(Scope::CLASS, cls.name);
		
		std::shared_ptr<NativeClassType> nat = std::dynamic_pointer_cast<NativeClassType>(class_symbol->type);
		native_types[cls.name] = nat;
		nat->class_ptr = class_symbol;
		
		std::cout << "native class: " << nat->to_string() << std::endl;

		for (auto& v : cls.variables) {
			auto nat_var = std::make_shared<VariableSymbol>(v);
			nat->class_ptr->scope->variables[v.name] = nat_var;
			std::cout << "\tnative var: " << nat_var->to_string() << std::endl;
		}
		
		for (auto& f : cls.methods) {
			auto nat_func = FunctionSymbol::create(f);
			class_symbol->functions[f.name] = nat_func;
			nat->functions.push_back(nat_func);
			std::cout << "\tnative func: " << nat_func->to_string() << std::endl;
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
}