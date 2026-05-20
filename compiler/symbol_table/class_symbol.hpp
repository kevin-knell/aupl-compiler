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

struct ClassSymbol : public std::enable_shared_from_this<ClassSymbol> {
private:
	struct Private{ explicit Private() = default; };

public:
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

	static ClassPtr create(const std::string& name) {
		return std::make_shared<ClassSymbol>(Private(), name);
	}

	static ClassPtr create(const vm::ClassBind& native_class_bind) {
		return std::make_shared<ClassSymbol>(Private(), native_class_bind);
	}

    ClassSymbol(Private, const std::string& name)
			: name(name),
			native_class_bind(nullptr),
			static_scope(Scope::create(Scope::STATIC_CLASS, "(static)" + name)),
			static_var(VariableSymbol::create(std::make_shared<StaticClassType>(name), "(static)" + name)),
			type(std::make_shared<ClassType>(name)) {}

    ClassSymbol(Private, const vm::ClassBind& native_class_bind)
			: name(native_class_bind.name),
			native_class_bind(&native_class_bind),
			static_var(VariableSymbol::create(std::make_shared<StaticClassType>(name), "(static)" + name)),
			type(std::make_shared<NativeClassType>(native_class_bind)),
			is_declared(true) {}
};

using ClassPtr = std::shared_ptr<ClassSymbol>;

}