#pragma once
#include "type.hpp"

namespace cmp
{

struct ClassType : public Type {
    std::string name;

    std::string to_string() const override { return name; }
    int get_size() const { return 256; };

	KIND get_kind() const override { return CLASS; };

	bool is_cpp_type(const std::string&) const override { return false; }

    ClassType(const std::string& n) : name(n) {}

	std::string to_cpp_type_str() override { return name; };

	bool default_store_shared() const override { return true; }
};

} // namespace cmp
