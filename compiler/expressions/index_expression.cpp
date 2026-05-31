#include "index_expression.hpp"
#include "forward_declarations.hpp"
#include "invalid_type.hpp"
#include "native_class_type.hpp"
#include "class_type.hpp"
#include "function_symbol.hpp"
#include "array_type.hpp"

namespace cmp {
std::vector<ExprPtr *> IndexExpression::get_expressions() {
	return { &left_expr, &index_expr };
}

std::string IndexExpression::to_string() const {
	return left_expr->to_string() + "[" + index_expr->to_string() + "]";
}

bool IndexExpression::is_unresolved_symbol() const {
	return left_expr->is_unresolved_symbol() || index_expr->is_unresolved_symbol();
}

void IndexExpression::resolve(NameAnalysisInfo &name_analysis_info) {
	left_expr->resolve(name_analysis_info);
	index_expr->resolve(name_analysis_info);
}

TypePtr IndexExpression::get_type() const {
	// TODO: get type from operator[]

	TypePtr left_type = left_expr->get_type();

	std::cout << left_type->to_string() << std::endl;

	if (left_type->get_kind() == Type::NATIVE_CLASS) {
		std::cout << "native class type!" << std::endl;
		auto native_class_type = std::static_pointer_cast<NativeClassType>(left_type);

		for (auto f : native_class_type->functions) {
			std::cout << f->to_string() << std::endl;
			if (f->name == "operator[]") {
				return f->return_type;
			}
		}
	} else if (left_type->get_kind() == Type::CLASS) {
		std::cout << "class type" << std::endl;
	} else if (left_type->get_kind() == Type::ARRAY) {
		std::cout << "array type" << std::endl;
		auto array_type = std::static_pointer_cast<ArrayType>(left_type);
		return array_type->base_type;
	}

	std::cerr << "Indexing type not found! " << to_string() << std::endl;
	
	return std::make_shared<InvalidType>("operator[]");
}

bool IndexExpression::is_constexpr() const {
	return left_expr->is_constexpr() && index_expr->is_constexpr();
}

vm::Value *IndexExpression::eval_constexpr() const {
	return nullptr;
}

}