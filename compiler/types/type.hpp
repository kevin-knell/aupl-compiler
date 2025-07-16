#pragma once
#include <string>
#include <memory>
#include "forward_declarations.hpp"

namespace cmp {

struct Type {
    virtual ~Type() = default;
    virtual std::string to_string() const = 0;
    virtual int get_size() const = 0;
};

using TypePtr = std::shared_ptr<Type>;

}