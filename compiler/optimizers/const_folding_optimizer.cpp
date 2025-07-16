#include "const_folding_optimizer.hpp"
#include "symbol_table.hpp"
#include "function_symbol.hpp"
#include "statement.hpp"
#include "expression.hpp"
#include "load_const_expression.hpp"

namespace cmp {

void cmp::ConstFoldingOptimizer::optimize(SymbolTable& st) const {
    std::cout << "\n" << "constant folding" << std::endl;

    for (auto [cn, cls] : st.classes) {
        for (auto [fn, f] : cls->functions) {
            for (auto stmt : f->scope->body) {
				if (stmt->is_volatile) continue;
                if (stmt->get_kind() == Statement::IF) continue;

                std::cout << stmt->to_string() << std::endl;

                for (auto p : stmt->get_expressions()) {
                    auto& expr = *p;
                    
                    if (expr->get_kind() == Expression::LOAD_CONST) continue;
					
                    if (expr->is_constexpr()) {
                        vm::Value* eval = expr->eval_constexpr();
						if (!eval) continue;
                        ExprPtr folded = std::make_shared<LoadConstExpression>(expr->get_type(), eval);
                        std::cout << expr->to_string() << " -> " << folded->to_string() << std::endl;
                        expr = folded;
                        std::cout << stmt->to_string() << std::endl;
                    }
                }
            }

            std::cout << f->to_string() << std::endl;
        }
    }
}

}