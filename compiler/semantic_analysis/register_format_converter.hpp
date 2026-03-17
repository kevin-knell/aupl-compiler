#pragma once

#include <iostream>

#include "statement_visitor.hpp"

#include "statement.hpp"
#include "declare_statement.hpp"
#include "assign_statement.hpp"
#include "conditional_jump_statement.hpp"
#include "return_statement.hpp"
#include "label_statement.hpp"
#include "expression_statement.hpp"

namespace cmp {

class RegisterFormatConverter : StatementVisitor {
private:
	SymbolTable& symbol_table;
	StmtVec statements;
    ScopePtr scope;

	void convert_scope(cmp::ScopePtr scope);
    void convert_to_register_format(ExprPtr expr, bool is_volatile);
	void replace_with_temp(ExprPtr& expr, bool is_volatile);

	void convert(DeclareStatement& stmt);
	void convert(AssignmentStatement& stmt);
	void convert(ConditionalJumpStatement& stmt);
	void convert(ReturnStatement& stmt);
	void convert(ExpressionStatement& stmt);
	void convert(LabelStatement& stmt);

	void visit(Statement& stmt) override {
		std::cerr << "Visited wrong type of statment: " << stmt.to_string() << std::endl;
		exit(1);
	}
	
	void visit(DeclareStatement& stmt) override { convert(stmt); }
	void visit(AssignmentStatement& stmt) override { convert(stmt); }
	void visit(ConditionalJumpStatement& stmt) override { convert(stmt); }
	void visit(ReturnStatement& stmt) override { convert(stmt); }
	void visit(ExpressionStatement& stmt) override { convert(stmt); }
	void visit(LabelStatement& stmt) override { convert(stmt); }
public:
    void convert_to_register_format();
    RegisterFormatConverter(SymbolTable& symbol_table) : symbol_table(symbol_table) {}
};

} // namespace cmp
