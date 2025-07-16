#include "erase_unused_variable_optimizer.hpp"

#include <set>
#include <algorithm>

#include "symbol_table.hpp"
#include "class_symbol.hpp"
#include "function_symbol.hpp"

#include "statement.hpp"
#include "declare_statement.hpp"

#include "expression.hpp"
#include "variable_expression.hpp"

namespace cmp {

void find_variable_references(std::vector<ExprPtr*> expressions, std::set<VarPtr>& result) {
    for (auto p : expressions) {
        auto expr = *p;

        if (expr->get_kind() == Expression::VARIABLE) {
            VarExprPtr var_expr = std::dynamic_pointer_cast<VariableExpression>(expr);
            result.emplace(var_expr->var);
            std::cout << "var ref: " << var_expr->to_string() << "\n" << std::endl;
        }

        find_variable_references(expr->get_expressions(), result);
    }
}

void cmp::EraseUnusedVariableOptimizer::optimize(SymbolTable &st) const {
    std::cout << "\n" << "Erasing unused variables" << std::endl;

    for (auto [cn, cls] : st.classes) {
        for (auto [fn, f] : cls->functions) {
            std::set<StmtPtr> unread_declarations;

            for (auto stmt : f->scope->body) {
                if (stmt->get_kind() == Statement::DECLARE) {
                    std::cout << "declaration: " << stmt->to_string() << std::endl;
                    unread_declarations.emplace(stmt);
                } else {
                    std::cout << stmt->to_string() << std::endl;
                }
                
                std::set<VarPtr> var_refs;
                find_variable_references(stmt->get_expressions(), var_refs);

                for (auto it = unread_declarations.begin(); it != unread_declarations.end(); ) {
                    auto decl = std::dynamic_pointer_cast<DeclareStatement>(*it);
					
					if (!decl->variable_symbol->initial_value) {
						++it;
						continue;
					}

                    if (var_refs.find(decl->variable_symbol) != var_refs.end()) {
                        it = unread_declarations.erase(it);
                    } else if (!decl->variable_symbol->initial_value->is_pure()) {
                        std::cout << "keep impure: " << stmt->to_string() << std::endl;
                        it = unread_declarations.erase(it);
                    } else if ((*it)->is_volatile) {
                        std::cout << "keep volatile: " << stmt->to_string() << std::endl;
                        it = unread_declarations.erase(it);
                    } else {
                        ++it;
                    }
                }
            }

            f->scope->body.erase(
                std::remove_if(f->scope->body.begin(), f->scope->body.end(),
                    [&unread_declarations](StmtPtr x) {
                        return unread_declarations.count(x) > 0;
                    }),
                f->scope->body.end()
            );

            for (const auto& stmt : unread_declarations) {
                auto decl = std::dynamic_pointer_cast<DeclareStatement>(stmt);
                if (decl && decl->variable_symbol) {
                    const std::string& var_name = decl->variable_symbol->name;
                    f->scope->variables.erase(var_name);
                }
            }

            std::cout << f->to_string() << std::endl;
        }
    }
}

}