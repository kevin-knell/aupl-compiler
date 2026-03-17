#include "register_format_converter.hpp"
#include "variable_expression.hpp"
#include "expression.hpp"
#include "scope.hpp"
#include "declare_statement.hpp"
#include "symbol_table.hpp"
#include "assign_statement.hpp"
#include "expression_statement.hpp"
#include "call_expression.hpp"
#include "return_statement.hpp"
#include "conditional_jump_statement.hpp"
#include "label_statement.hpp"
#include <color.hpp>
#include <assert.h>

#ifdef RF_DEBUG_VERBOSE
#define RF_DEBUG_PRINT(m_text) std::cout << m_text << std::endl
#define RF_DEBUG_PRINT_V(m_text) RF_DEBUG_PRINT(m_text)
#define RF_DEBUG_PRINT_NV(m_text)
#elifdef RF_DEBUG
#define RF_DEBUG_PRINT(m_text) std::cout << m_text << std::endl
#define RF_DEBUG_PRINT_V(m_text)
#define RF_DEBUG_PRINT_NV(m_text) RF_DEBUG_PRINT(m_text)
#else
#define RF_DEBUG_PRINT(m_text)
#define RF_DEBUG_PRINT_V(m_text)
#define RF_DEBUG_PRINT_NV(m_text)
#endif

namespace cmp {

void RegisterFormatConverter::convert_scope(cmp::ScopePtr current_scope) {
	RF_DEBUG_PRINT(current_scope->get_full_name());

	scope = current_scope;

	auto old_body = current_scope->body;
	auto new_body = StmtVec();

	for (auto stmt : old_body) {
		RF_DEBUG_PRINT("statement: " << stmt->to_string());

		statements.clear();
		stmt->accept(*this);
		statements.push_back(stmt);

		RF_DEBUG_PRINT_V(statements.size() << " new statements:");
			
		for (StmtPtr s : statements) {
			RF_DEBUG_PRINT_V("\t" << s->to_string());
		}

		new_body.insert(new_body.end(), statements.begin(), statements.end());

		RF_DEBUG_PRINT_V("statement changed to: " << stmt->to_string());
	}

	current_scope->body = new_body;

	RF_DEBUG_PRINT("");

	for (auto& lower : scope->lower_scopes) {
		if (auto lower_scope = lower.lock())
			convert_scope(lower_scope);
		else
			std::cout << "Warning: lower scope is expired" << std::endl;
	}
}

void RegisterFormatConverter::convert_to_register_format(ExprPtr expr, bool is_volatile) {
	//RF_DEBUG_PRINT("expression: " << expr->to_string());

    for (auto p : expr->get_expressions()) {
        auto& sub_expr = *p;
        assert(sub_expr);
		
		if (sub_expr->get_level() == 0) continue;

		//RF_DEBUG_PRINT("\treplacing level " << sub_expr->get_level() << ": " << sub_expr->to_string());
        replace_with_temp(sub_expr, is_volatile);
    }

	//RF_DEBUG_PRINT("set to expression: " << expr->to_string() << std::endl);
}

void RegisterFormatConverter::replace_with_temp(ExprPtr& expr, bool is_volatile) {
	// create temp
	TypePtr type = expr->get_type();
	VarPtr tmp = scope->get_temp(type, expr);
	ExprPtr tmp_expr = std::make_shared<VariableExpression>(tmp);
	StmtPtr tmp_decl = std::make_shared<DeclareStatement>(tmp);
	tmp_decl->is_volatile = is_volatile;
	tmp->scope = scope;
	//RF_DEBUG_PRINT_V("\tcreated temp: " << tmp->name_to_string());

	// recursion
	convert_to_register_format(expr, is_volatile);

	// replace
	RF_DEBUG_PRINT("\treplacing " << expr->to_string() << " with " << tmp_expr->to_string());
	expr = tmp_expr;
	statements.push_back(tmp_decl);

}
// ================================================================================================
// Statement Conversions
// ================================================================================================
void RegisterFormatConverter::convert(DeclareStatement& stmt) {
	auto& expr = stmt.variable_symbol->initial_value;
	convert_to_register_format(expr, stmt.is_volatile);
}

void RegisterFormatConverter::convert(AssignmentStatement& stmt) {
	auto& expr = stmt.expr_right;
	convert_to_register_format(expr, stmt.is_volatile);
}

void RegisterFormatConverter::convert(ConditionalJumpStatement& stmt) {
	if (!stmt.condition) {
		return;
	}

	auto& expr = stmt.condition;

	if (expr->get_kind() != Expression::VARIABLE
			&& expr->get_kind() != Expression::BINARY) {
		replace_with_temp(expr, stmt.is_volatile);
	} else {
		convert_to_register_format(stmt.condition, stmt.is_volatile);
	}
}

void RegisterFormatConverter::convert(ReturnStatement& stmt) {
	auto& expr = stmt.return_value;
	replace_with_temp(expr, stmt.is_volatile);
}

void RegisterFormatConverter::convert(ExpressionStatement& stmt) {
	auto& expr = stmt.expression;
	convert_to_register_format(expr, stmt.is_volatile);
}

void RegisterFormatConverter::convert(LabelStatement& stmt) {
}

// ================================================================================================
// Public
// ================================================================================================
void RegisterFormatConverter::convert_to_register_format() {
    for (auto& [class_name, cls] : symbol_table.classes) {
		if (cls->native_class_bind) continue;
		
        RF_DEBUG_PRINT(C_KEYWORD("Class ") << cls->name);

        for (const auto& [func_name, f] : cls->functions) {
			convert_scope(f->scope);

			RF_DEBUG_PRINT("function changed to: " << std::flush << f->to_string());
        }
        
		RF_DEBUG_PRINT("");
    }
}

}