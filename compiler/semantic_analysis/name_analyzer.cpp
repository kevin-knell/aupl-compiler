#include "name_analyzer.hpp"

#include <iostream>

#include "expression.hpp"
#include "load_const_expression.hpp"
#include "variable_expression.hpp"
#include "binary_op_expression.hpp"
#include "string_literal_expression.hpp"
#include "unary_op_expression.hpp"
#include "call_expression.hpp"
#include "tuple_expression.hpp"
#include "index_expression.hpp"

#include "statement.hpp"
#include "text_color.hpp"
#include "assign_statement.hpp"
#include "compiler_error.hpp"

#define NA_DEBUG_PRINT(m_text) \
	if (NameAnalyzer::na_debug_print) { \
		std::cout << m_text << std::endl; \
	}

#define NA_DEBUG_PRINT_V(m_text) \
	if (NameAnalyzer::na_debug_print_verbose) { \
		NA_DEBUG_PRINT(m_text); \
	}

#define NA_DEBUG_PRINT_NV(m_text) \
	if (!NameAnalyzer::na_debug_print_verbose) { \
		NA_DEBUG_PRINT(m_text); \
	}

namespace cmp {

bool NameAnalyzer::na_debug_print = false;
bool NameAnalyzer::na_debug_print_verbose = false;

void NameAnalyzer::resolve_expressions(std::vector<ExprPtr*> expressions) {
    for (ExprPtr* p : expressions) {
        ExprPtr expr = *p;
		
		COMPILER_ASSERT(expr, "Invalid Expression");

        if (expr->is_unresolved_symbol()) {
			NA_DEBUG_PRINT("\t" << expr->to_string());

			expr->accept(*this);

			if (expr->is_unresolved_symbol()) {
				symbol_table.add_error(
					expr->source_location,
					"cannot resolve: " + expr->to_string(),
					Error::ERROR
				);
				continue;
			}
        }
        
		//resolve_expressions(expr->get_expressions());
    }
}

void NameAnalyzer::resolve_variables() {
	NA_DEBUG_PRINT("===== ===== ===== ===== ===== ===== ===== ===== ===== =====");
	NA_DEBUG_PRINT("Resolve Variables");
	NA_DEBUG_PRINT("===== ===== ===== ===== ===== ===== ===== ===== ===== =====");
    for (auto& [class_name, cls] : symbol_table.classes) {
		if (cls->native_class_bind) continue;

        NA_DEBUG_PRINT(C_KEYWORD("Class ") << cls->name);
        for (auto& [func_name, f] : cls->functions) {
			if (f->is_override) {
				f->return_type = cls->parent->functions[func_name]->return_type;
			}

			std::function<void(cmp::ScopePtr)> resolve_scope = [&](cmp::ScopePtr sc) {
				NA_DEBUG_PRINT(sc->get_full_name() << " " << C_BRACE_L);
				
				for (auto& stmt : sc->body) {
					NA_DEBUG_PRINT(stmt->to_string());

					NameAnalysisContext _na_context = NameAnalysisContext(cls, f, sc);
					na_context = &_na_context;
					resolve_expressions(stmt->get_expressions());
					
					if (stmt->get_kind() == Statement::ASSIGN) {
						auto assign_stmt = std::dynamic_pointer_cast<AssignmentStatement>(stmt);
						ExprPtr* expr_left = &assign_stmt->expr_left;
						resolve_expressions({expr_left});
					}
				}

				NA_DEBUG_PRINT(C_BRACE_R);

				COMPILER_ASSERT(sc, "");
                
				for (auto& lower : sc->lower_scopes) {
					if (auto lower_scope = lower.lock())
						resolve_scope(lower_scope);
					else
						std::cout << "Warning: lower scope is expired" << std::endl;
                }
			};

			resolve_scope(f->scope);
        }
		NA_DEBUG_PRINT("")
    }

	NA_DEBUG_PRINT("===== ===== ===== ===== ===== ===== ===== ===== ===== =====");
	NA_DEBUG_PRINT("Resolve Variables Completed");
	NA_DEBUG_PRINT("===== ===== ===== ===== ===== ===== ===== ===== ===== =====");
}

void NameAnalyzer::visit(VariableExpression& expr) {
	//std::cout << "resolve var expr: " << to_string() << std::endl;

	ScopePtr scope;

	if (expr.obj_expr) {
		expr.obj_expr->accept(*this);
		
		if (expr.obj_expr->is_unresolved_symbol()) {
			std::cerr << "cannot resolve: " << expr.obj_expr->to_string() << std::endl;
			return;
		}

		TypePtr obj_type = expr.obj_expr->get_type();
		
		/*
		if (obj_type->get_kind() == Type::CLASS) {
			auto class_type = std::dynamic_pointer_cast<ClassType>(obj_type);
			auto classes = name_analysis_info.symbol_table.classes;
			auto cls = classes.find(class_type->name);
			scope = Scope::find_scope(cls->second->scope, name);
		} else
		*/
		
		if (obj_type->is_pointer_type()) {
			Type& inner_type = obj_type->get_inner_type();

			if (expr.obj_expr->get_kind() == Expression::VARIABLE) {
				VarExprPtr obj_var_expr = std::static_pointer_cast<VariableExpression>(expr.obj_expr);
				obj_var_expr->must_be_dereferenced = true;
			}
			
			ClassPtr cls;

			if (inner_type.get_kind() == Type::CLASS) {
				ClassType& class_type = dynamic_cast<ClassType&>(inner_type);

				if (class_type.class_ptr) {
					cls = class_type.class_ptr;
				} else {
					auto classes = symbol_table.classes;
					auto it = classes.find(class_type.name);

					if (it != classes.end()) {
						cls = it->second;
					} else {
						std::cerr << "Class not found: " << class_type.to_string() << std::endl;
						return;
					}
				}
			}
			
			scope = Scope::find_scope(cls->scope, expr.name);
			//std::cout << scope->get_full_name() << std::endl;
		} else {
			ClassPtr cls;

			if (obj_type->get_kind() == Type::CLASS) {
				ClassType& class_type = dynamic_cast<ClassType&>(*obj_type);
				if (class_type.class_ptr) {
					cls = class_type.class_ptr;
				} else {
					auto it = symbol_table.classes.find(class_type.name);
					cls = it->second;
				}
			} else {
				std::cerr << "invalid obj_type: " << obj_type->to_string() << std::endl;
				return;
			}
			
			scope = Scope::find_scope(cls->scope, expr.name);
		}
		COMPILER_ASSERT(scope, "");
	} else {
		scope = Scope::find_scope(na_context->scope, expr.name);
	}
	
	if (scope) {
        expr.var = scope->variables[expr.name];
		if (!expr.obj_expr && scope->type == Scope::CLASS) {
			auto obj_var_expr = std::make_shared<VariableExpression>("this");
			obj_var_expr->must_be_dereferenced = true;
			expr.obj_expr = obj_var_expr;
		}
    } else {
        auto it = symbol_table.classes.find(expr.name);
        if (it != symbol_table.classes.end()) {
            expr.var = it->second->static_var;
        }
    }
}

void NameAnalyzer::visit(UnaryExpression& expr) {
	expr.expr->accept(*this);
}

void NameAnalyzer::visit(BinaryExpression& expr) {
	expr.left->accept(*this);
	expr.right->accept(*this);
}

void NameAnalyzer::visit(TupleExpression& expr) {
	for (ExprPtr e : expr.expressions) {
		e->accept(*this);
	}
}

void NameAnalyzer::visit(IndexExpression& expr) {
	expr.left_expr->accept(*this);
	expr.index_expr->accept(*this);
}


}