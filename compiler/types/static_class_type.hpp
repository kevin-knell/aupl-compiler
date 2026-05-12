#pragma once
#include "type.hpp"

namespace cmp
{

struct StaticClassType final : public Type {
    std::string name;

    std::string to_string() const override { return name; }
    size_t get_size() const override { return 1; };

	KIND get_kind() const override { return STATIC_CLASS; };
    StaticClassType(const std::string& n) : name(n) {}

	bool is_cpp_type(const std::string&) const override { return false; }
};

} // namespace cmp
