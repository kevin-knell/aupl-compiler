#include "name_analyzer.hpp"

#include <iostream>

#include "statement.hpp"
#include "expression.hpp"
#include "color.hpp"
#include "assign_statement.hpp"
#include "access_expression.hpp"

#ifdef NA_DEBUG
#define NA_DEBUG_PRINT(m_text) std::cout << m_text << std::endl;
#else
#define NA_DEBUG_PRINT(m_text)
#endif

namespace cmp {

static void resolve_expressions(NameAnalysisInfo& name_analysis_info, std::vector<ExprPtr*> expressions) {
    for (auto p : expressions) {
        auto expr = *p;
		
		if (!expr) {
			std::cout << "no expression!" << std::endl;
			continue;
		}

        if (expr->is_unresolved_symbol()) {
            NA_DEBUG_PRINT("\t" << expr->to_string());
            NameAnalysisInfo name_analysis_info_base = name_analysis_info;
			
			expr->resolve(name_analysis_info_base);
			if (expr->is_unresolved_symbol()) {
				std::cout << "cannot resolve: " << expr->to_string() << std::endl;
				exit(1);
			}
        }
        
		resolve_expressions(name_analysis_info, expr->get_expressions());
    }
}

void NameAnalyzer::resolve_variables() const {
    for (auto& [class_name, cls] : symbol_table.classes) {
		if (cls->native_class_bind) continue;

        NA_DEBUG_PRINT(C_KEYWORD("Class ") << cls->name);
        for (auto& [func_name, f] : cls->functions) {
			std::function<void(cmp::ScopePtr)> resolve_scope = [&](cmp::ScopePtr scope) {
				NA_DEBUG_PRINT(scope->get_full_name() << " " << C_BRACE_L);
				
				for (auto& stmt : scope->body) {
					NA_DEBUG_PRINT(stmt->to_string());

					NameAnalysisInfo name_analysis_info{.symbol_table = symbol_table, .cls = cls, .f = f, .scope = scope};
					resolve_expressions(name_analysis_info, stmt->get_expressions());
					
					if (stmt->get_kind() == Statement::ASSIGN) {
						auto assign_stmt = std::dynamic_pointer_cast<AssignmentStatement>(stmt);
						ExprPtr* expr_left = &assign_stmt->expr_left;
						resolve_expressions(name_analysis_info, {expr_left});
					}
				}

				NA_DEBUG_PRINT(C_BRACE_R);
                
				for (auto& lower : scope->lower_scopes) {
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
}

}