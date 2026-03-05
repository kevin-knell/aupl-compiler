#pragma once
#include "type.hpp"
#include <vector>
#include <memory>
#include <string>

namespace cmp {

struct TupleType : public Type {
    std::vector<TypePtr> types;

    TupleType(std::vector<TypePtr> types);

    std::string to_string() const override;

    int get_size() const override;

	KIND get_kind() const override;

	bool is_cpp_type(const std::string& cpp_type) const override { return false; }
};

} // namespace cmp
