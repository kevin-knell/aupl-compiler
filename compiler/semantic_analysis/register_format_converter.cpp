#include "register_format_converter.hpp"
#include "variable_expression.hpp"
#include "expression.hpp"
#include "scope.hpp"
#include "declare_statement.hpp"
#include "symbol_table.hpp"
#include <color.hpp>


namespace cmp {

void RegisterFormatConverter::convert_to_register_format(RegisterFormatInfo& rfi, ExprPtr expr, bool is_volatile) {
    for (auto p : expr->get_expressions()) {
        auto& sub_expr = *p;
        if (!sub_expr) std::cerr << "invalid sub_expr in " << expr->to_string() << std::endl;
        if (sub_expr->get_level() == 0) continue;
        std::cout << "replacing level " << sub_expr->get_level() << ": " << sub_expr->to_string() << std::endl;

        // create temp
        TypePtr type = sub_expr->get_type();
        VarPtr tmp = rfi.scope->get_temp(type, sub_expr);
        ExprPtr tmp_expr = std::make_shared<VariableExpression>(tmp);
        StmtPtr tmp_decl = std::make_shared<DeclareStatement>(tmp);
		tmp_decl->is_volatile = is_volatile;

        // recursion
        convert_to_register_format(rfi, sub_expr, is_volatile);

        // replace
        sub_expr = tmp_expr;
        rfi.statements.push_back(tmp_decl);
    }
}

void RegisterFormatConverter::convert_to_register_format(SymbolTable &symbol_table) {
    for (auto& [class_name, cls] : symbol_table.classes) {
        std::cout << C_KEYWORD("Class ") << cls->name << std::endl;
        for (auto& [func_name, f] : cls->functions) {
            std::cout << class_name << "." << f->name << "(...)" << std::endl;
            for (size_t i = 0; i < f->scope->body.size(); ++i) {
                auto stmt = f->scope->body[i];

                for (auto p : stmt->get_expressions()) {
                    auto& expr = *p;

                    RegisterFormatInfo rfi{.statements = {}, .scope = f->scope};

                    if ((stmt->get_kind() == Statement::IF
                            || stmt->get_kind() == Statement::RETURN) && expr->get_kind() != Expression::VARIABLE) {
                        TypePtr type = expr->get_type();
                        VarPtr tmp = rfi.scope->get_temp(type, expr);
                        ExprPtr tmp_expr = std::make_shared<VariableExpression>(tmp);
                        StmtPtr tmp_decl = std::make_shared<DeclareStatement>(tmp);
						tmp_decl->is_volatile = stmt->is_volatile;

                        convert_to_register_format(rfi, expr, stmt->is_volatile);

                        expr = tmp_expr;
                        rfi.statements.push_back(tmp_decl);
                    } else {
                        convert_to_register_format(rfi, expr, stmt->is_volatile);
                    }

                    // std::cout << expr->to_string() << std::endl;

                    std::cout << std::endl;
                    for (auto s : rfi.statements) {
                        std::cout << s->to_string() << std::endl;
                    }

                    // Insert rfi.statements before stmt (i.e., at position i)
                    f->scope->body.insert(f->scope->body.begin() + i, rfi.statements.begin(), rfi.statements.end());

                    // Move the index past the inserted statements
                    i += rfi.statements.size();
                }
                std::cout << "\t" << stmt->to_string() << std::endl;
            }
            std::cout << f->to_string() << std::endl;
        }
        std::cout << std::endl;
    }
}

}