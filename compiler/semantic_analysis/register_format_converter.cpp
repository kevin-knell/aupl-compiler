#include "register_format_converter.hpp"
#include "variable_expression.hpp"
#include "expression.hpp"
#include "scope.hpp"
#include "declare_statement.hpp"
#include "symbol_table.hpp"
#include "assign_statement.hpp"
#include "expression_statement.hpp"
#include "call_expression.hpp"
#include "conditional_jump_statement.hpp"
#include <color.hpp>

#define DEBUG
#ifdef DEBUG
#define RF_DEBUG_PRINT(m_text) std::cout << m_text << std::endl
#else
#define RF_DEBUG_PRINT(m_text)
#endif

namespace cmp {

void RegisterFormatConverter::convert_to_register_format(RegisterFormatInfo& rfi, ExprPtr expr, bool is_volatile) {
    for (auto p : expr->get_expressions()) {
        auto& sub_expr = *p;
        if (!sub_expr) std::cerr << "invalid sub_expr in " << expr->to_string() << std::endl;
        if (sub_expr->get_level() == 0) continue;
        RF_DEBUG_PRINT("\treplacing level " << sub_expr->get_level() << ": " << sub_expr->to_string());

        // create temp
        TypePtr type = sub_expr->get_type();
        VarPtr tmp = rfi.scope->get_temp(type, sub_expr);
        ExprPtr tmp_expr = std::make_shared<VariableExpression>(tmp);
        StmtPtr tmp_decl = std::make_shared<DeclareStatement>(tmp);
		tmp_decl->is_volatile = is_volatile;
		tmp->scope = rfi.scope;
		RF_DEBUG_PRINT("\tcreated temp: " << tmp->name_to_string());

        // recursion
        convert_to_register_format(rfi, sub_expr, is_volatile);

        // replace
		RF_DEBUG_PRINT("\treplacing " << sub_expr->to_string() << " with " << tmp_expr->to_string());
        sub_expr = tmp_expr;
        rfi.statements.push_back(tmp_decl);
    }
}

void RegisterFormatConverter::convert_to_register_format(SymbolTable &symbol_table) {
    for (auto& [class_name, cls] : symbol_table.classes) {
		if (cls->native_class_bind) continue;
		
        RF_DEBUG_PRINT(C_KEYWORD("Class ") << cls->name);
        for (auto& [func_name, f] : cls->functions) {
			std::function<void(cmp::ScopePtr)> convert_scope = [&](cmp::ScopePtr scope) {
				RF_DEBUG_PRINT(scope->get_full_name());
				for (size_t i = 0; i < scope->body.size(); ++i) {
					auto stmt = scope->body[i];

					RF_DEBUG_PRINT("statement: " << stmt->to_string());

					for (auto p : stmt->get_expressions()) {
						auto& expr = *p;

						RF_DEBUG_PRINT("expression: " << expr->to_string());

						RegisterFormatInfo rfi{.statements = {}, .scope = scope};

						bool requires_variable = false;

						if (expr->get_kind() != Expression::VARIABLE) {
							if (stmt->get_kind() == Statement::IF) {
								if (expr->get_kind() != Expression::BINARY) {
									requires_variable = true;
								}
							} else if (stmt->get_kind() == Statement::RETURN) {
								requires_variable = true;
							} else if (stmt->get_kind() == Statement::ASSIGN) {
								auto assign_stmt = std::dynamic_pointer_cast<AssignmentStatement>(stmt);
								auto expr_left = std::dynamic_pointer_cast<VariableExpression>(assign_stmt->expr_left);
								if (expr_left) {
									auto var = expr_left->var;
									if (var->is_static) {
										requires_variable = true;
									}
								}
							}
						}

						RF_DEBUG_PRINT((requires_variable ? "requires variable" : "does not require variable"));

						if (requires_variable) {
							TypePtr type = expr->get_type();
							VarPtr tmp = rfi.scope->get_temp(type, expr);
							ExprPtr tmp_expr = std::make_shared<VariableExpression>(tmp);
							StmtPtr tmp_decl = std::make_shared<DeclareStatement>(tmp);
							tmp_decl->is_volatile = stmt->is_volatile;
							tmp->scope = rfi.scope;

							convert_to_register_format(rfi, expr, stmt->is_volatile);

							expr = tmp_expr;
							rfi.statements.push_back(tmp_decl);
						} else {
							convert_to_register_format(rfi, expr, stmt->is_volatile);
						}

						RF_DEBUG_PRINT("set to expression: " << expr->to_string() << std::endl);
						RF_DEBUG_PRINT(rfi.statements.size() << " new statements:");
						for (auto s : rfi.statements) {
							RF_DEBUG_PRINT("\t" << s->to_string());
						}

						// Insert rfi.statements before stmt (i.e., at position i)
						scope->body.insert(scope->body.begin() + i, rfi.statements.begin(), rfi.statements.end());
						// Move the index past the inserted statements
						i += rfi.statements.size();
					}
					RF_DEBUG_PRINT("statement changed to: " << stmt->to_string());
				}
				//RF_DEBUG_PRINT("function changed to: " << f->to_string());

				std::cout << std::endl;
                
				for (auto& lower : scope->lower_scopes) {
					if (auto lower_scope = lower.lock())
						convert_scope(lower_scope);
					else
						std::cout << "Warning: lower scope is expired" << std::endl;
                }
			};

			convert_scope(f->scope);
        }
        RF_DEBUG_PRINT("");
    }
}

}