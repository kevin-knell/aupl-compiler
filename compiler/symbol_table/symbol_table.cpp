#include "symbol_table.hpp"
#include "native_class_type.hpp"
#include "static_class_type.hpp"
#include "class_db.hpp"

namespace cmp {

SymbolTable::SymbolTable(vm::ClassDB &db) {
	global_scope = std::make_shared<Scope>(Scope::SCOPE_TYPE::GLOBAL);
	global_scope->name = "global";

	// TODO: add global scope

	for (auto& cls : db.classes) {
		auto class_symbol = std::make_shared<ClassSymbol>(cls);
		class_symbol->static_var = std::make_shared<VariableSymbol>(std::make_shared<StaticClassType>(cls.name), cls.name, nullptr);
		
		auto nat = std::make_shared<NativeClassType>(cls);
		native_types[cls.name] = nat;
		class_symbol->type = nat;
		
		std::cout << "native class: " << nat->to_string() << std::endl;
		
		for (auto& f : cls.methods) {
			auto nat_func = std::make_shared<FunctionSymbol>(f);
			class_symbol->functions[f.name] = nat_func;
			nat->functions.push_back(nat_func);
			std::cout << "\tnative func: " << nat_func->to_string() << std::endl;
			//std::cout << "\t\t" << nat_func->method_pair->arg_count << std::endl;
			if (f.is_global) {
				global_native_functions.push_back(nat_func);
				std::cout << "\tglobal native func: " << nat_func->to_string() << std::endl;
			}
		}

		classes[cls.name] = class_symbol;
	}
}

}