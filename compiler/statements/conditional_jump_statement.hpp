#pragma once
#include "statement.hpp"
#include "expression.hpp"
#include <string>

namespace cmp {

class Label;
using LabelPtr = std::shared_ptr<Label>;

enum class CJ_KIND {
	IF,
	IF_RETURN,

	WHILE,
	WHILE_RETURN
};

class ConditionalJumpStatement : public Statement {
public:
	const CJ_KIND cj_kind;
    ExprPtr condition;
    LabelPtr if_label;
    LabelPtr else_label;

    ConditionalJumpStatement(CJ_KIND cj_kind, ExprPtr cond, LabelPtr if_lbl, LabelPtr else_lbl)
        : cj_kind(cj_kind), condition(cond), if_label(if_lbl), else_label(else_lbl) {};

	OVERRIDE_ACCEPT_STATMENT_VISITOR

    KIND get_kind() const override { return KIND::IF; } // Or add JUMP if you want a new kind
    std::string to_string() const override;
    std::vector<ExprPtr*> get_expressions() override {
		if (condition) {
        	return { &condition };
		} else {
			return {};
		}
    }
};

} // namespace cmp
