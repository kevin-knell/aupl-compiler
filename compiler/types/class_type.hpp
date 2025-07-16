#pragma once
#include "type.hpp"

namespace cmp
{

struct ClassType : public Type {
    std::string name;

    std::string to_string() const override { return name; }
    int get_size() const { return 1; };
private:
    ClassType(const std::string& n) : name(n) {}

    friend void SymbolBuilder::parse_class();
};

} // namespace cmp
