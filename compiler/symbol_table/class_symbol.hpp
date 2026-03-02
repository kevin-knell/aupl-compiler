#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>
#include "variable_symbol.hpp"
#include "function_symbol.hpp"
#include "error.hpp"
#include "class_db.hpp"
#include "invalid_type.hpp"


namespace cmp {

struct ClassSymbol {
    std::string name;
    std::shared_ptr<ClassSymbol> parent;

	const bool is_native;
	vm::ClassBind native_class_bind;
    
	ScopePtr static_scope;
    ScopePtr scope;
    
	std::map<std::string, FuncPtr> functions;
    std::vector<Error> errors;
    
	VarPtr static_var;
    TypePtr type;
    bool is_declared;

    ClassSymbol(const std::string& name) : name(name), is_native(false) {}
    ClassSymbol(const vm::ClassBind& native_class_bind)
			: name(native_class_bind.name), is_native(true), native_class_bind(native_class_bind), is_declared(true) {}
};

using ClassPtr = std::shared_ptr<ClassSymbol>;

}