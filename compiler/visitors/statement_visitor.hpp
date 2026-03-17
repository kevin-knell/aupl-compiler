#pragma once

namespace cmp
{

class Statement;
class DeclareStatement;
class AssignmentStatement;
class ConditionalJumpStatement;
class ReturnStatement;
class LabelStatement;
class ExpressionStatement;

class StatementVisitor {
private:
	virtual void visit(Statement& stmt) = 0;
	virtual void visit(DeclareStatement& stmt) = 0;
	virtual void visit(AssignmentStatement& stmt) = 0;
	virtual void visit(ConditionalJumpStatement& stmt) = 0;
	virtual void visit(ReturnStatement& stmt) = 0;
	virtual void visit(LabelStatement& stmt) = 0;
	virtual void visit(ExpressionStatement& stmt) = 0;

	friend class Statement;
	friend class DeclareStatement;
	friend class AssignmentStatement;
	friend class ConditionalJumpStatement;
	friend class ReturnStatement;
	friend class LabelStatement;
	friend class ExpressionStatement;
};

} // namespace cmp
