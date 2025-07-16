#pragma once
#include <type.hpp>

#include <vector>

namespace cmp
{

struct GenericType : public Type {
    TypePtr base;
    std::vector<TypePtr> parameters;

    GenericType(TypePtr base_type, std::vector<TypePtr> params)
        : base(std::move(base_type)), parameters(std::move(params)) {}

    std::string to_string() const override {
        std::string result = base->to_string() + "<";
        for (size_t i = 0; i < parameters.size(); ++i) {
            if (i > 0) result += ", ";
            result += parameters[i]->to_string();
        }
        return result + ">";
    }

    int get_size() { return 1; };
};

} // namespace cmp
