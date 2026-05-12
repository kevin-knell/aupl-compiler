#pragma once
#include "optimizer.hpp"

namespace cmp {

struct EraseUnusedVariableOptimizer final : public Optimizer {
    void optimize(SymbolTable& st) const override;
	~EraseUnusedVariableOptimizer() override = default;
};
    
} // namespace cmp
