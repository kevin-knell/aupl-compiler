#include "tuple_type.hpp"

namespace cmp {

TupleType::TupleType(std::vector<TypePtr> types)
    : types(std::move(types)) {}

std::string TupleType::to_string() const {
    std::string result = "(";
    for (size_t i = 0; i < types.size(); ++i) {
        if (i > 0) result += ", ";
        result += types[i]->to_string();
    }
    return result + ")";
}

size_t TupleType::get_size() const {
	size_t result = 0;

	for (auto t : types) {
		result += t->get_size();
	}

    return result;
}

Type::KIND TupleType::get_kind() const {
	return TUPLE;
}

} // namespace cmp
