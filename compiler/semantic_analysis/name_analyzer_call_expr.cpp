#include "name_analyzer.hpp"
#include "call_expression.hpp"
#include "variable_expression.hpp"

namespace cmp {

namespace {
	FuncVec get_candidates(CallExpression& expr, FuncVec functions) {
		FuncVec candidates;
		for (auto f2 : functions) {
			// add candidates
			if (f2->name != expr.name) continue;
			if (f2->parameters.size() != expr.arguments.size()) continue;
			
			candidates.push_back(f2);
		}
		return candidates;
	}

	FuncVec get_secondary_candidates(CallExpression& expr, FuncVec candidates) {
		FuncVec secondary_candidates;

		for (auto f2 : candidates) {
			bool has_different_type = false;
			bool has_incompatible_type = false;

			if (f2->method_pair) {
				FuncPtr nat_func = f2;

				for (size_t i = 0; i < nat_func->method_pair->arg_types.size(); i++) {
					std::string cpp_type_name = nat_func->method_pair->arg_types[i];
					auto arg = expr.arguments[i];
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

	bool resolve_from_secondary_candidates(SymbolTable& symbol_table, CallExpression& expr, FuncVec secondary_candidates) {
		if (secondary_candidates.size() == 1) {
			expr.f = secondary_candidates.front();
			return true;
		}
		
		if (secondary_candidates.size() > 1) {
			symbol_table.add_error(
				expr.source_location,
				"function call " + expr.to_string() + " is ambigous!",
				Error::ERROR
			);
			
			for (auto nat_func : secondary_candidates) {
				std::cerr << nat_func->to_string() << std::endl;
			}

			return false;
		}

		COMPILER_ASSERT(secondary_candidates.empty(), "");
		symbol_table.add_error(
			expr.source_location,
			"no function matches: " + expr.to_string(),
			Error::ERROR
		);
		return false;
	}
}

void NameAnalyzer::visit(CallExpression& expr) {
	for (auto arg : expr.arguments) {
		arg->accept(*this);
	}

	if (!expr.obj_expr) {
		// native constructor
		auto& native_types = symbol_table.native_types;
		auto native_class_it = native_types.find(expr.name);
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
			
			FuncVec candidates = get_candidates(expr, constructors);
			FuncVec secondary_candidates = get_secondary_candidates(expr, candidates);
			bool success = resolve_from_secondary_candidates(symbol_table, expr, secondary_candidates);

			if (!success) {
				symbol_table.add_error(expr.source_location, "invalid native constructor", Error::ERROR);
			}
			return;
		}

		// global native func
		FuncVec& global_native_functions = symbol_table.global_native_functions;
		FuncVec global_native_functions_same_name;
		std::copy_if(
			global_native_functions.cbegin(),
			global_native_functions.cend(),
			std::back_inserter(global_native_functions_same_name),
			[&](FuncPtr f2) {
				return f2->name == expr.name;
			}
		);

		if (!global_native_functions_same_name.empty()) {
			FuncVec candidates = get_candidates(expr, global_native_functions_same_name);
			FuncVec secondary_candidates = get_secondary_candidates(expr, candidates);
			bool success = resolve_from_secondary_candidates(symbol_table, expr, secondary_candidates);

			if (!success) {
				symbol_table.add_error(expr.source_location, "invalid global native function", Error::ERROR);
			}
			return;
		}

		// constructor
		auto classes = symbol_table.classes;
		auto class_it = classes.find(expr.name);
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
			
			FuncVec candidates = get_candidates(expr, constructors);
			FuncVec secondary_candidates = get_secondary_candidates(expr, candidates);
			bool success = resolve_from_secondary_candidates(symbol_table, expr, secondary_candidates);

			if (!success) {
				symbol_table.add_error(expr.source_location, "invalid constructor", Error::ERROR);
			}
			return;
		}

		// call function in same class
		FuncVec functions = na_context->class_ptr->get_functions();
		FuncVec candidates = get_candidates(expr, functions);
		FuncVec secondary_candidates = get_secondary_candidates(expr, candidates);
		bool success = resolve_from_secondary_candidates(symbol_table, expr, secondary_candidates);

		if (!success) {
			symbol_table.add_error(expr.source_location, "invalid function", Error::ERROR);
		}
		return;
	} else {
		expr.obj_expr->accept(*this);

		// TODO: get functions from type

		switch (expr.obj_expr->get_kind()) {
			case Expression::VARIABLE: {
				//std::cout << "is variable: " << expr.obj_expr->to_string() << std::endl;

				VarExprPtr var_expr = std::dynamic_pointer_cast<VariableExpression>(expr.obj_expr);
				VarPtr var = var_expr->var;
				TypePtr access_type = var_expr->get_type();

				const Type* obj_type = &access_type->get_inner_type();

				if (access_type->is_pointer_type()) {
					var_expr->must_be_dereferenced = true;
				}

				switch (obj_type->get_kind()) {
					case Type::STATIC_CLASS: {
						//std::cout << "is static class" << std::endl;
						auto static_class_type = reinterpret_cast<const StaticClassType*>(obj_type);
						auto it = symbol_table.classes.find(static_class_type->name);
						if (it != symbol_table.classes.end()) {
							ClassPtr cls = it->second;
							auto func_it = cls->functions.find(expr.name);
							if (func_it != cls->functions.end()) {
								//std::cout << "function is: " << func_it->second->name << std::endl;
								expr.f = func_it->second;
								return;
							}
						}
						std::cout << C_ERROR("Class not found") << std::endl;
						return;
					}
					case Type::CLASS: {
						const ClassType* class_type = static_cast<const ClassType*>(obj_type);
						FuncVec functions;
						ClassPtr cls;

						if (class_type->class_ptr) {
							cls = class_type->class_ptr;
							functions = cls->get_functions();
						} else {
							auto classes = symbol_table.classes;
							auto class_it = classes.find(class_type->name);

							if (class_it == classes.end()) {
								std::cerr << "did not find class: " << class_type->name << std::endl;
								return;
							}

							functions = class_it->second->get_functions();
						}

						FuncVec candidates = get_candidates(expr, functions);
						FuncVec secondary_candidates = get_secondary_candidates(expr, candidates);
						bool success = resolve_from_secondary_candidates(symbol_table, expr, secondary_candidates);

						if (!success) {
							symbol_table.add_error(expr.source_location, "Method in class not found", Error::ERROR);
						}
						return;
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

}