#include "call_expression.hpp"
#include "type.hpp"
#include "function_symbol.hpp"
#include "invalid_type.hpp"
#include "class_symbol.hpp"
#include "color.hpp"
#include "symbol_table.hpp"
#include "native_class_type.hpp"
#include "variable_expression.hpp"
#include "static_class_type.hpp"
#include "bytecode_generator.hpp"

namespace cmp {

std::string CallExpression::to_string() const {
    std::string result;

	result += obj_expr ? obj_expr->to_string() + "." : "";
	
	result += f ? C_IDENTIFIER(f->name) : C_UNRES(name);

    result += "(";

    for (auto it = arguments.begin(); it != arguments.end(); ++it) {
        if (it != arguments.begin()) {
            result += ", ";
        }

        result += it->get()->to_string();
    }

    result += ")";

    return result;
}

bool CallExpression::is_unresolved_symbol() const {
    return !f;
}

void CallExpression::resolve(NameAnalysisInfo& name_analysis_info) {
	for (auto arg : arguments) {
		arg->resolve(name_analysis_info);
	}

	if (!obj_expr) {
		// native constructor
		auto native_types = name_analysis_info.symbol_table.native_types;
		auto it = native_types.find(name);
		if (it != native_types.end()) {
			//std::cout << "function is type: " << it->second->to_string() << std::endl;
			auto nat = std::dynamic_pointer_cast<NativeClassType>(it->second);

			// add candidates
			FuncVec candidates;
			for (auto nat_func : nat->functions) {
				if (nat_func->name == name
						&& nat_func->method_pair->arg_count == arguments.size()) {
					candidates.push_back(nat_func);
					//std::cout << "candidate: " << nat_func->to_string() << std::endl;
				}
			}

			for (auto nat_func : candidates) {
				bool wrong_type = false;
				
				for (size_t i = 0; i < nat_func->method_pair->arg_types.size(); i++) {
					std::string cpp_type_name = nat_func->method_pair->arg_types[i];
					TypePtr type = arguments[i]->get_type();

					if (!type->is_cpp_type(cpp_type_name)) {
						wrong_type = true;
						break;
					}
				}

				if (!wrong_type) {
					f = nat_func;
					return;
				}
			}

			std::cout << "no valid constructor!" << std::endl;
			return;
		}

		// global native func
		auto global_native_functions = name_analysis_info.symbol_table.global_native_functions;
		FuncVec candidates;
		for (auto nat_func : global_native_functions) {
			// add candidates
			if (nat_func->name == name
					&& nat_func->method_pair->arg_count == arguments.size()) {
				candidates.push_back(nat_func);
				//std::cout << "candidate: " << nat_func->to_string() << std::endl;
			}
		}

		for (auto nat_func : candidates) {
			bool wrong_type = false;
			
			for (size_t i = 0; i < nat_func->method_pair->arg_types.size(); i++) {
				std::string cpp_type_name = nat_func->method_pair->arg_types[i];
				TypePtr type = arguments[i]->get_type();

				if (!type->is_cpp_type(cpp_type_name)) {
					wrong_type = true;
					break;
				}
			}

			if (!wrong_type) {
				f = nat_func;
				return;
			}
		}

		// constructor
		auto classes = name_analysis_info.symbol_table.classes;
		auto class_it = classes.find(name);
		if (class_it != classes.end()) {
			auto constructed_class = class_it->second;
			auto functions = constructed_class->functions;
			for (auto func_it = functions.begin(); func_it != functions.end(); func_it++) {
				if (func_it->first != "(constructor)") continue;
				
				auto func = func_it->second;
				if (func->parameters.size() == arguments.size()) {
					f = func;
					return;
				}
			}

			std::cerr << "no valid constructor found!" << std::endl;
			return;
		}

		// call function in same class
		if (!name_analysis_info.cls) {
			std::cout << "no class" << std::endl;
			return;
		}

		for (auto [n2, f2] : name_analysis_info.cls->functions) {
			if (n2 == name) {
				f = f2;
				return;
			}
		}
	} else {
		obj_expr->resolve(name_analysis_info);

		switch (obj_expr->get_kind()) {
			case Expression::VARIABLE: {
				auto var_expr = std::dynamic_pointer_cast<VariableExpression>(obj_expr);
				auto var = var_expr->var;
				auto obj_type = var_expr->get_type();
				switch (obj_type->get_kind()) {
					case Type::STATIC_CLASS: {
						//std::cout << "is static class" << std::endl;
						auto static_class_type = std::dynamic_pointer_cast<StaticClassType>(obj_type);
						auto it = name_analysis_info.symbol_table.classes.find(static_class_type->name);
						if (it != name_analysis_info.symbol_table.classes.end()) {
							auto class_ptr = it->second;
							auto func_it = class_ptr->functions.find(name);
							if (func_it != class_ptr->functions.end()) {
								//std::cout << "function is: " << func_it->second->name << std::endl;
								f = func_it->second;
								return;
							}
						}
						std::cout << C_ERROR("Class not found") << std::endl;
						return;
					}
					case Type::NATIVE_CLASS: {
						//std::cout << "is native class" << std::endl;
						auto native_class_type = std::dynamic_pointer_cast<NativeClassType>(obj_type);
						for (auto f2 : native_class_type->functions) {
							if (f2->name == name
									&& f2->method_pair->arg_count == arguments.size()) {
								f = f2;
								return;
							}
						}
						break;
					}
					default: {}
				}
			}
			case Expression::CALL: {

			}
			default: {}
		}
	}
}

std::vector<ExprPtr *> CallExpression::get_expressions() {
    std::vector<ExprPtr *> result;

    for (auto& expr : arguments) {
        result.push_back(&expr);
    }

    return result;
}

bool CallExpression::is_pure() const {
    return f->is_pure;
}

TypePtr CallExpression::get_type() const {
    return (f && f->return_type) ? f->return_type : InvalidType::make("unknown call: " + name);
}

}