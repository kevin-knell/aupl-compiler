#pragma once
#include "forward_declarations.hpp"

namespace cmp {

struct Optimizer {
    virtual void optimize(SymbolTable& st) const = 0;
};

}