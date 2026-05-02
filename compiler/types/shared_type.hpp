#pragma once
#include "type.hpp"

namespace cmp {

struct SharedType : public Type {
	TypePtr type;
	
	SharedType(TypePtr type);

    std::string to_string() const override;

    int get_size() const override;

	KIND get_kind() const override;

	bool is_cpp_type(const std::string& cpp_type) const override { return type->is_cpp_type(cpp_type); }

	bool is_pointer_type() override { return true; }
	Type& get_inner_type() override { return *type; }

	std::string to_cpp_type_str() override;
};

} // namespace cmp
