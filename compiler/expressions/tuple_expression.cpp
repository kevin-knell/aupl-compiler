#include "tuple_expression.hpp"

#include "tuple_type.hpp"
#include <cstring>
#include <assert.h>

namespace cmp
{
	std::vector<ExprPtr *> TupleExpression::get_expressions() {
		std::vector<ExprPtr *> result;

		for (auto& e : expressions) {
			result.push_back(&e);
		}

		return result;
	}

	std::string TupleExpression::to_string() const {
    std::string result = "(";
    for (size_t i = 0; i < expressions.size(); ++i) {
        if (i > 0) result += ", ";
        result += expressions[i]->to_string();
    }
    return result + ")";
}

TypePtr TupleExpression::get_type() const {
	TypeVec types;

	for (auto e : expressions) {
		types.push_back(e->get_type());
	}

	return std::make_shared<TupleType>(types);
}

bool TupleExpression::is_constexpr() const {
	for (auto& e : expressions) {
		if (!e->is_constexpr()) return false;
	}
	
	return true;
}

vm::Value* TupleExpression::eval_constexpr() const {
	std::shared_ptr<TupleType> type = std::dynamic_pointer_cast<TupleType>(get_type());
	size_t type_size = type->get_size();
	vm::Value* result = new vm::Value[type->get_size()];

	size_t i = 0;
	for (auto e : expressions) {
		vm::Value* v = e->eval_constexpr();
		size_t size = e->get_type()->get_size();
		std::memcpy(result + i, v, size);
		i += size;
	}
	std::cout << i << " " << type_size << std::endl;
	assert(i == type_size);

	return result;
}

} // namespace cmp
