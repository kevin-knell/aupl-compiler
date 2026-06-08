#include "node_composition_expression.hpp"
#include "text_color.hpp"
#include "type.hpp"

#include <sstream>
#include <algorithm>

namespace cmp {

std::string NodeCompositionExpression::to_string() const {
	std::stringstream result;

	// name
	result << "\\" << constructor_call_expr->to_string() << " ";

	// content
	if (content_tuple_expr->get_kind() != Expression::TUPLE) {
		result << content_tuple_expr->to_string();
		return result.str();
	}

	ExprVec content = std::static_pointer_cast<TupleExpression>(content_tuple_expr)->expressions;

	if (content.empty()) {
		result << "{}";
		return result.str();
	}

	result << "{" << "\n";

	for (auto it = content.begin(); it != content.end(); ++it) {
		ExprPtr expr = *it;

		if (it != content.begin()) {
			result << ",\n";
		}

		result << "\t" << expr->to_string();
	}

	result << "\n}";
	
	return result.str();
}

std::vector<ExprPtr*> NodeCompositionExpression::get_expressions() {
	return {
		&constructor_call_expr,
		&content_tuple_expr
	};
}

bool NodeCompositionExpression::is_unresolved_symbol() const {
	return (
		constructor_call_expr->is_unresolved_symbol()
		|| content_tuple_expr->is_unresolved_symbol()
	);
}

TypePtr NodeCompositionExpression::get_type() const {
	return constructor_call_expr->get_type();
}

bool NodeCompositionExpression::is_constexpr() const {
	return false; // TODO
}
vm::Value *NodeCompositionExpression::eval_constexpr() const {
	return nullptr; // TODO
}
}