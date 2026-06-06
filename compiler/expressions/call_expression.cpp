#include "call_expression.hpp"
#include "type.hpp"
#include "function_symbol.hpp"
#include "invalid_type.hpp"
#include "class_symbol.hpp"
#include "text_color.hpp"
#include "symbol_table.hpp"
#include "variable_expression.hpp"
#include "static_class_type.hpp"
#include "bytecode_generator.hpp"
#include "class_type.hpp"
#include "forward_declarations.hpp"
#include "pointer_type.hpp"

#include "compiler_error.hpp"

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

int CallExpression::get_level() {
	int super_level = Expression::get_level();
	if (super_level == 0) {
		return 1;
	} else {
		return super_level;
	}
}

bool CallExpression::is_unresolved_symbol() const {
    return !f || (obj_expr && obj_expr->is_unresolved_symbol());
}

void CallExpression::resolve(NameAnalysisInfo& name_analysis_info) {
	for (auto arg : arguments) {
		arg->resolve(name_analysis_info);
	}

	if (!obj_expr) {
		// native constructor
		auto& native_types = name_analysis_info.symbol_table.native_types;
		auto native_class_it = native_types.find(name);
		if (native_class_it != native_types.end()) {
			FuncVec constructors;
			ClassPtr constructed_class = native_class_it->second->class_ptr;
			FuncVec functions = constructed_class->get_functions();

			std::copy_if(
				functions.cbegin(),
				functions.cend(),
				std::back_inserter(constructors),
				[&](FuncPtr f2) {
					return f2->is_constructor;
				}
			);
			
			FuncVec candidates = get_candidates(constructors);
			FuncVec secondary_candidates = get_secondary_candidates(candidates);
			bool success = resolve_from_secondary_candidates(secondary_candidates, name_analysis_info);

			if (!success) {
				name_analysis_info.symbol_table.add_error(source_location, "invalid native constructor", Error::ERROR);
			}
			return;
		}

		// global native func
		FuncVec& global_native_functions = name_analysis_info.symbol_table.global_native_functions;
		FuncVec global_native_functions_same_name;
		std::copy_if(
			global_native_functions.cbegin(),
			global_native_functions.cend(),
			std::back_inserter(global_native_functions_same_name),
			[&](FuncPtr f2) {
				return f2->name == name;
			}
		);

		if (!global_native_functions_same_name.empty()) {
			FuncVec candidates = get_candidates(global_native_functions_same_name);
			FuncVec secondary_candidates = get_secondary_candidates(candidates);
			bool success = resolve_from_secondary_candidates(secondary_candidates, name_analysis_info);

			if (!success) {
				name_analysis_info.symbol_table.add_error(source_location, "invalid global native function", Error::ERROR);
			}
			return;
		}

		// constructor
		auto classes = name_analysis_info.symbol_table.classes;
		auto class_it = classes.find(name);
		if (class_it != classes.end()) {
			FuncVec constructors;
			ClassPtr constructed_class = class_it->second;
			FuncVec functions = constructed_class->get_functions();

			std::copy_if(
				functions.cbegin(),
				functions.cend(),
				std::back_inserter(constructors),
				[&](FuncPtr f2) {
					return f2->is_constructor;
				}
			);
			
			FuncVec candidates = get_candidates(constructors);
			FuncVec secondary_candidates = get_secondary_candidates(candidates);
			bool success = resolve_from_secondary_candidates(secondary_candidates, name_analysis_info);

			if (!success) {
				name_analysis_info.symbol_table.add_error(source_location, "invalid constructor", Error::ERROR);
			}
			return;
		}

		// call function in same class
		COMPILER_ASSERT(name_analysis_info.cls, "");

		FuncVec functions = name_analysis_info.cls->get_functions();
		FuncVec candidates = get_candidates(functions);
		FuncVec secondary_candidates = get_secondary_candidates(candidates);
		bool success = resolve_from_secondary_candidates(secondary_candidates, name_analysis_info);

		if (!success) {
			name_analysis_info.symbol_table.add_error(source_location, "invalid function", Error::ERROR);
		}
		return;
	} else {
		obj_expr->resolve(name_analysis_info);

		// TODO: get functions from type

		switch (obj_expr->get_kind()) {
			case Expression::VARIABLE: {
				//std::cout << "is variable: " << obj_expr->to_string() << std::endl;

				auto var_expr = std::dynamic_pointer_cast<VariableExpression>(obj_expr);
				auto var = var_expr->var;
				auto access_type = var_expr->get_type();

				const Type* obj_type;

				if (access_type->is_pointer_type()) {
					obj_type = &access_type->get_inner_type();
					var_expr->must_be_dereferenced = true;
				} else {
					obj_type = access_type.get();
					//std::cerr << "invalid type for call: " << access_type->to_string() << std::endl;
					//return;
				}

				switch (obj_type->get_kind()) {
					case Type::STATIC_CLASS: {
						//std::cout << "is static class" << std::endl;
						auto static_class_type = reinterpret_cast<const StaticClassType*>(obj_type);
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
					case Type::CLASS: {
						const ClassType* class_type = static_cast<const ClassType*>(obj_type);
						std::map<std::string, FuncPtr> functions;
						ClassPtr class_ptr;

						if (class_type->class_ptr) {
							class_ptr = class_type->class_ptr;
							functions = class_ptr->functions;
						} else {
							auto classes = name_analysis_info.symbol_table.classes;
							auto class_it = classes.find(class_type->name);

							if (class_it == classes.end()) {
								std::cerr << "did not find class: " << class_type->name << std::endl;
								return;
							}

							functions = class_it->second->functions;
						}
						
						for (auto [fn, f2] : functions) {
							if (f2->name == name
									&& f2->parameters.size() == arguments.size()) {
								f = f2;
								return;
							}
						}
						
						std::cerr << "Method in class not found: " << name << std::endl;
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

FuncVec CallExpression::get_candidates(FuncVec functions) {
	FuncVec candidates;
	for (auto f2 : functions) {
		// add candidates
		if (f2->name != name) continue;
		if (f2->parameters.size() != arguments.size()) continue;
		
		candidates.push_back(f2);
	}
	return candidates;
}

FuncVec CallExpression::get_secondary_candidates(FuncVec candidates) {
	FuncVec secondary_candidates;

	for (auto f2 : candidates) {
		bool has_different_type = false;
		bool has_incompatible_type = false;

		if (f2->method_pair) {
			FuncPtr nat_func = f2;

			for (size_t i = 0; i < nat_func->method_pair->arg_types.size(); i++) {
				std::string cpp_type_name = nat_func->method_pair->arg_types[i];
				auto arg = arguments[i];
				TypePtr type = arg->get_type();

				if (!type) {
					std::cerr << "type == null_ptr! " << arg->to_string() << std::endl;
					has_different_type = true;
					continue;
				}

				COMPILER_ASSERT(type, "");

				if (!type->is_cpp_type(cpp_type_name)) {
					//std::cout << type->to_string() << type->get_kind() << (type->get_inner_type().get_kind()) << " is not " << cpp_type_name << std::endl;
					has_different_type = true;

					if (!type->is_convertable_to_cpp_type(cpp_type_name)) {
						has_incompatible_type = true;
						//std::cout << type->to_string() << " not convertable to " << cpp_type_name << std::endl;
					}
				}
			}
		} else {
			// TODO
		}

		if (!has_different_type) {
			return { f2 };
		}

		if (!has_incompatible_type) {
			secondary_candidates.push_back(f2);
		}
	}

	return secondary_candidates;
}

bool CallExpression::resolve_from_secondary_candidates(FuncVec secondary_candidates, NameAnalysisInfo& name_analysis_info) {
	if (secondary_candidates.size() == 1) {
		f = secondary_candidates.front();
		return true;
	}
	
	if (secondary_candidates.size() > 1) {
		name_analysis_info.symbol_table.add_error(
			source_location,
			"function call " + to_string() + " is ambigous!",
			Error::ERROR
		);
		
		for (auto nat_func : secondary_candidates) {
			std::cerr << nat_func->to_string() << std::endl;
		}

		return false;
	}

	COMPILER_ASSERT(secondary_candidates.empty(), "");
	name_analysis_info.symbol_table.add_error(
		source_location,
		"no function matches: " + to_string(),
		Error::ERROR
	);
	return false;
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
    if (f && f->return_type) {
		if (f->is_constructor) {
			return std::make_shared<ClassType>(f->name);
		} else {
			return f->return_type;
		}
	}
	
	return InvalidType::make("unknown call: " + name);
}

}