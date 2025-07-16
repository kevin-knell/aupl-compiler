#pragma once
#include "optimizer.hpp"

namespace cmp {

struct ConstFoldingOptimizer : public Optimizer {
    void optimize(SymbolTable& st) const override;
};

} // namespace cmp
