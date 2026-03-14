#pragma once
#include "forward_declarations.hpp"

namespace cmp
{

class Expression;
class VariableExpression;
class UnaryExpression;
class BinaryExpression;
class LoadConstExpression;
class CallExpression;
class AccessExpression;
class TupleExpression;
class StringLiteralExpression;

using VarExprPtr = std::shared_ptr<VariableExpression>;

class ExpressionVisitor {
private:
	virtual void visit(Expression& expr) {};
	virtual void visit(VariableExpression& expr) {};
	virtual void visit(UnaryExpression& expr) {};
	virtual void visit(BinaryExpression& expr) {};
	virtual void visit(LoadConstExpression& expr) {};
	virtual void visit(CallExpression& expr) {};
	virtual void visit(AccessExpression& expr) {};
	virtual void visit(TupleExpression& expr) {};
	virtual void visit(StringLiteralExpression& expr) {};

	virtual void visit(Expression& expr, VarExprPtr var_expr) {};
	virtual void visit(VariableExpression& expr, VarExprPtr var_expr) {};
	virtual void visit(UnaryExpression& expr, VarExprPtr var_expr) {};
	virtual void visit(BinaryExpression& expr, VarExprPtr var_expr) {};
	virtual void visit(LoadConstExpression& expr, VarExprPtr var_expr) {};
	virtual void visit(CallExpression& expr, VarExprPtr var_expr) {};
	virtual void visit(AccessExpression& expr, VarExprPtr var_expr) {};
	virtual void visit(TupleExpression& expr, VarExprPtr var_expr) {};
	virtual void visit(StringLiteralExpression& expr, VarExprPtr var_expr) {};

	friend class Expression;
	friend class VariableExpression;
	friend class UnaryExpression;
	friend class BinaryExpression;
	friend class LoadConstExpression;
	friend class CallExpression;
	friend class AccessExpression;
	friend class TupleExpression;
	friend class StringLiteralExpression;
};

} // namespace cmp
