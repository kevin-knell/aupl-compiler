#pragma once
#include "type.hpp"

namespace cmp {

struct PointerType final : public Type {
	TypePtr type;
	
	PointerType(TypePtr type);

    std::string to_string() const override;

    size_t get_size() const override;

	KIND get_kind() const override;

	bool is_cpp_type(const std::string&) const override { return false; }

	bool is_pointer_type() override { return true; }
	Type& get_inner_type() override { return *type; }

	std::string to_cpp_type_str() override;
};

} // namespace cmp
