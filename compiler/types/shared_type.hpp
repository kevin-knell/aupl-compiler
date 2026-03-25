#pragma once
#include "type.hpp"

namespace cmp {

struct SharedType : public Type {
	TypePtr type;
	
	SharedType(TypePtr type);

    std::string to_string() const override;

    int get_size() const override;

	KIND get_kind() const override;

	bool is_cpp_type(const std::string&) const override { return false; }
};

} // namespace cmp
