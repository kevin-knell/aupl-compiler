#include "variable_expression.hpp"
#include "scope.hpp"
#include "instructions.hpp"
#include "symbol_table.hpp"
#include "variable_symbol.hpp"
#include "invalid_type.hpp"
#include "native_class_type.hpp"
#include "color.hpp"
#include "shared_type.hpp"
#include <assert.h>

namespace cmp {
std::vector<ExprPtr *> VariableExpression::get_expressions() {
	return obj_expr ? std::vector({&obj_expr}) : std::vector<ExprPtr *>();
}

std::string VariableExpression::to_string() const {
	std::string obj_string = obj_expr ? obj_expr->to_string() + "." : "";
    std::string name_string = is_unresolved_symbol() == true ? C_UNRES(name) : var->name_to_string();
	return obj_string + name_string;
}

bool VariableExpression::is_unresolved_symbol() const {
    return var == nullptr;
}

void VariableExpression::resolve(NameAnalysisInfo& name_analysis_info) {
	ScopePtr scope;

	if (obj_expr) {
		obj_expr->resolve(name_analysis_info);
		
		if (obj_expr->is_unresolved_symbol()) {
			std::cerr << "cannot resolve: " << obj_expr->to_string() << std::endl;
			return;
		}

		auto obj_type = obj_expr->get_type();
		
		/*
		if (obj_type->get_kind() == Type::CLASS) {
			auto class_type = std::dynamic_pointer_cast<ClassType>(obj_type);
			auto classes = name_analysis_info.symbol_table.classes;
			auto cls = classes.find(class_type->name);
			scope = Scope::find_scope(cls->second->scope, name);
		} else
		*/
		
		if (obj_type->is_pointer_type()) {
			const Type& inner_type = obj_type->get_inner_type();
			
			ClassPtr cls;

			if (inner_type.get_kind() == Type::CLASS) {
				const ClassType* class_type = reinterpret_cast<const ClassType*>(&inner_type);
				auto classes = name_analysis_info.symbol_table.classes;
				auto it = classes.find(class_type->name);

				if (it != classes.end()) {
					cls = it->second;
				} else {
					std::cerr << "Class not found: " << class_type->to_string() << std::endl;
					return;
				}
			} else if (inner_type.get_kind() == Type::NATIVE_CLASS) {
				const NativeClassType* class_type = reinterpret_cast<const NativeClassType*>(&inner_type);
				cls = class_type->class_ptr;
			}
			
			scope = Scope::find_scope(cls->scope, name);
		} else {
			return;
		}
		assert(scope);
	} else {
		scope = Scope::find_scope(name_analysis_info.scope, name);
	}
	
	if (scope) {
        var = scope->variables[name];
		if (scope->type == Scope::CLASS) {
			obj_expr = std::make_shared<VariableExpression>("this");
		}
    } else {
        auto it = name_analysis_info.symbol_table.classes.find(name);
        if (it != name_analysis_info.symbol_table.classes.end()) {
            var = it->second->static_var;
            name_analysis_info.cls = it->second;
        }
    }
}

TypePtr VariableExpression::get_type() const {
    return var ? var->type : InvalidType::make("unknown variable: " + name);
}

bool VariableExpression::is_constexpr() const {
    if (!var->initial_value) return false;
    else return var->initial_value->is_constexpr();
}

vm::Value* VariableExpression::eval_constexpr() const {
    if (!var->initial_value) return nullptr;
    else return var->initial_value->eval_constexpr();
}

}