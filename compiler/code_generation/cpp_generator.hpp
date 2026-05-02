#pragma once
#include <fstream>

#include "symbol_table.hpp"

#include "statement_visitor.hpp"
#include "expression_visitor.hpp"

namespace cmp {

class CppCodeGenerator : StatementVisitor, ExpressionVisitor {
private:
	const SymbolTable& symbol_table;
	ClassPtr current_class;
	FuncPtr current_function;
	ScopePtr scope;

	std::stringstream hpp_includes;
	std::stringstream hpp_forward_declarations;
	std::stringstream hpp_classes;
	std::stringstream hpp_global_functions;

	std::stringstream cpp_includes;
	std::stringstream cpp_classes;
	std::stringstream cpp_main;

public:
	CppCodeGenerator(const SymbolTable& symbol_table) : symbol_table(symbol_table) {}
	void generate_cpp_code(std::ofstream& hpp_file, std::ofstream& cpp_file);

private:
	void iterate_scope();

	void visit(Statement&) override { throw std::runtime_error("unsuported function!"); }
	void visit(DeclareStatement& stmt) override;
	void visit(AssignmentStatement& stmt) override;
	void visit(ConditionalJumpStatement& stmt) override;
	void visit(ReturnStatement& stmt) override;
	void visit(LabelStatement& stmt) override;
	void visit(ExpressionStatement& stmt) override;

	void visit(Expression&) override { throw std::runtime_error("unsuported function!"); }
	void visit(VariableExpression& expr) override;
	void visit(UnaryExpression& expr) override;
	void visit(BinaryExpression& expr) override;
	void visit(LoadConstExpression& expr) override;
	void visit(CallExpression& expr) override;
	void visit(TupleExpression& expr) override { (void) expr; throw std::runtime_error("unsuported function!"); }
	void visit(StringLiteralExpression& expr) override;
};

}