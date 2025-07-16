#pragma once
#include "type.hpp"

namespace cmp
{

struct InvalidType : public Type {
    std::string name;

    static TypePtr make(const std::string& n) {
        return std::make_shared<InvalidType>(n);
    }

    InvalidType(const std::string& n) : name(n) {}

    std::string to_string() const override { return "(invalid)" + name; }
    int get_size() const { return 1; };
};

} // namespace cmp
