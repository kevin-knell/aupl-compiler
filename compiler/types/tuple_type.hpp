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
};

} // namespace cmp
