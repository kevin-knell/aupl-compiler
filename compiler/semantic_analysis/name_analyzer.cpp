#include "name_analyzer.hpp"

#include <iostream>

#include "statement.hpp"
#include "expression.hpp"
#include "color.hpp"

namespace cmp {

static void resolve_expressions(NameAnalysisInfo& name_analysis_info, std::vector<ExprPtr*> expressions) {
    for (auto p : expressions) {
        auto expr = *p;
        if (expr->is_unresolved_symbol()) {
            std::cout << expr->to_string() << std::endl;
            NameAnalysisInfo name_analysis_info_base = name_analysis_info;
            expr->resolve(name_analysis_info_base);
            if (expr->is_unresolved_symbol()) {
                std::cout << "cannot resolve: " << expr->to_string() << std::endl;
            }
        } else {
            resolve_expressions(name_analysis_info, expr->get_expressions());
        }
    }
}

void NameAnalyzer::resolve_variables() const {
    for (auto& [class_name, cls] : symbol_table.classes) {
        std::cout << C_KEYWORD("Class ") << cls->name << std::endl;
        for (auto& [func_name, f] : cls->functions) {
            std::cout << class_name << "." << f->name << "(...)" << std::endl;
            for (auto& stmt : f->scope->body) {
                NameAnalysisInfo name_analysis_info{.symbol_table = symbol_table, .cls = cls, .f = f};
                resolve_expressions(name_analysis_info, stmt->get_expressions());
            }
        }
        std::cout << std::endl;
    }
}

}