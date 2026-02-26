#include "symbol_table.hpp"
#include "native_class_type.hpp"
#include "class_db.hpp"

namespace cmp {

SymbolTable::SymbolTable(vm::ClassDB &db) {
	for (auto& cls : db.classes) {
		auto nat = std::make_shared<NativeClassType>(cls);
		native_types[cls.name] = nat;
		std::cout << "native class: " << nat->to_string() << std::endl;
		for (auto& f : cls.methods) {
			auto nat_func = std::make_shared<FunctionSymbol>(f);
			nat->functions.push_back(nat_func);
			std::cout << "\tnative func: " << nat_func->to_string() << std::endl;
			//std::cout << "\t\t" << nat_func->method_pair->arg_count << std::endl;
			if (f.is_global) {
				global_native_functions.push_back(nat_func);
				std::cout << "\tglobal native func: " << nat_func->to_string() << std::endl;
			}
		}
	}
}

}