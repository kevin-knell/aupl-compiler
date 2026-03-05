#pragma once
#include "type.hpp"

namespace cmp
{

struct ClassType : public Type {
    std::string name;

    std::string to_string() const override { return name; }
    int get_size() const { return 1; };

	KIND get_kind() const override { return CLASS; };

	bool is_cpp_type(const std::string& cpp_type) const override { return false; }
private:
    ClassType(const std::string& n) : name(n) {}

    friend void SymbolBuilder::parse_class();
};

} // namespace cmp
