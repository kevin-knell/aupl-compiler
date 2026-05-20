#pragma once
#include "type.hpp"
//#include "expression.hpp"
#include <string>
#include "forward_declarations.hpp"
#include "class_db.hpp"

namespace cmp {

struct VariableSymbol : public std::enable_shared_from_this<VariableSymbol> {
private:
	struct Private{ explicit Private() = default; };

public:
    enum class MemoryType {
        VALUE,      // value directly in memory
        SHARED,     // shared_ptr
        MANUAL,     // new, delete
        STATIC      // when an object exists exactly once (?)
    };

    bool is_public = false;
    bool is_static = false;
    bool is_const = false;
    TypePtr type;
    std::string name;
    ExprPtr initial_value;
    ScopePtr scope;

	static VarPtr create() {
		return std::make_shared<VariableSymbol>(Private());
	}

	static VarPtr create(TypePtr type, const std::string& name, ExprPtr initial_value = nullptr) {
		return std::make_shared<VariableSymbol>(Private(), type, name, initial_value);
	}

	static VarPtr create(vm::VariableBind& v) {
		return std::make_shared<VariableSymbol>(Private(), v);
	}

    VariableSymbol(Private) {};

    VariableSymbol(Private, TypePtr type, const std::string& name, ExprPtr initial_value = nullptr)
        : type(std::move(type)), name(name), initial_value(std::move(initial_value)) {}
	
	VariableSymbol(Private, vm::VariableBind& v);
    
    std::string to_string() const;
    std::string name_to_string() const;

	size_t get_index() const;
};

}