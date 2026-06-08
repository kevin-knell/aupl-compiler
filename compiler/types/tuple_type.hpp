#pragma once
#include "type.hpp"
#include <vector>
#include <memory>
#include <string>

namespace cmp {

struct TupleType final : public Type {
    std::vector<TypePtr> types;

    TupleType(std::vector<TypePtr> types);

	DEFINE_SELF(TupleType)

    std::string to_string() const override;

    size_t get_size() const override;

	KIND get_kind() const override;

	std::string to_cpp_type_str() override { return "auto"; }

	bool is_cpp_type(const std::string&) const override { return false; }
	bool is_convertable_to_cpp_type(const std::string&) const override { return false; }
};

} // namespace cmp
