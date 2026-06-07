#pragma once
#include "symbol_table.hpp"
#include "expression_visitor.hpp"
#include "compiler_error.hpp"

namespace cmp
{
    
class NameAnalyzer : public ExpressionVisitor {
private:
	struct NameAnalysisContext {
		ClassPtr class_ptr;
		FuncPtr func;
		ScopePtr scope;

		NameAnalysisContext(
			const ClassPtr class_ptr,
			const FuncPtr func,
			const ScopePtr scope)
				:	class_ptr(class_ptr),
					func(func),
					scope(scope) {}
	};

	SymbolTable& symbol_table;
	const NameAnalysisContext* na_context;

	void resolve_expressions(std::vector<ExprPtr*> expressions);

public:
	static bool na_debug_print;
	static bool na_debug_print_verbose;

    NameAnalyzer(SymbolTable& symbol_table) : symbol_table(symbol_table) {}

    void resolve_variables();

	void visit(Expression&) override { COMPILER_ERR("Unsupported function"); };
	void visit(VariableExpression& expr) override;
	void visit(UnaryExpression& expr) override;
	void visit(BinaryExpression& expr) override;
	void visit(LoadConstExpression&) override {};
	void visit(CallExpression& expr) override;
	void visit(TupleExpression& expr) override;
	void visit(StringLiteralExpression&) override {};
	void visit(IndexExpression& expr) override;
};

} // namespace cmp
