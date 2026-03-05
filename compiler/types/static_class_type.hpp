#pragma once
#include "type.hpp"
#include "symbol_builder.hpp"

namespace cmp
{

struct StaticClassType : public Type {
    std::string name;

    std::string to_string() const override { return name; }
    int get_size() const { return 1; };

	KIND get_kind() const override { return STATIC_CLASS; };
    StaticClassType(const std::string& n) : name(n) {}

	bool is_cpp_type(const std::string& cpp_type) const override { return false; }
};

} // namespace cmp
