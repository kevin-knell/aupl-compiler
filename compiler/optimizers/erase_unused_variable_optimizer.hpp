#pragma once
#include "optimizer.hpp"

namespace cmp {

struct EraseUnusedVariableOptimizer : public Optimizer {
    void optimize(SymbolTable& st) const override;
};
    
} // namespace cmp
