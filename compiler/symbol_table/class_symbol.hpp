#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>
#include "variable_symbol.hpp"
#include "function_symbol.hpp"
#include "error.hpp"
#include "class_db.hpp"
#include "static_class_type.hpp"
#include "class_type.hpp"
#include "native_class_type.hpp"
#include "invalid_type.hpp"


namespace cmp {

struct ClassSymbol {
    std::string name;
    std::shared_ptr<ClassSymbol> parent;
	
	const vm::ClassBind* native_class_bind;
    
	ScopePtr static_scope;
    ScopePtr scope;
    
	std::map<std::string, FuncPtr> functions;
    std::vector<Error> errors;
    
	const VarPtr static_var;
    const TypePtr type;
    bool is_declared;

    ClassSymbol(const std::string& name)
			: name(name),
			native_class_bind(nullptr),
			static_scope(std::make_shared<Scope>(Scope::STATIC_CLASS, "(static)" + name)),
			static_var(std::make_shared<VariableSymbol>(std::make_shared<StaticClassType>(name), "(static)" + name)),
			type(std::make_shared<ClassType>(name)) {}

    ClassSymbol(const vm::ClassBind& native_class_bind)
			: name(native_class_bind.name),
			native_class_bind(&native_class_bind),
			static_var(std::make_shared<VariableSymbol>(std::make_shared<StaticClassType>(name), "(static)" + name)),
			type(std::make_shared<NativeClassType>(native_class_bind)),
			is_declared(true)
			 {}
};

using ClassPtr = std::shared_ptr<ClassSymbol>;

}