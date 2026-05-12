#pragma once
#include "type.hpp"
#include <vector>
#include <memory>
#include <string>

namespace cmp {

struct TupleType final : public Type {
    std::vector<TypePtr> types;

    TupleType(std::vector<TypePtr> types);

    std::string to_string() const override;

    size_t get_size() const override;

	KIND get_kind() const override;

	bool is_cpp_type(const std::string&) const override { return false; }
};

} // namespace cmp
