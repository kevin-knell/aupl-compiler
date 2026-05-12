#pragma once
#include "optimizer.hpp"

namespace cmp {

struct ConstFoldingOptimizer final : public Optimizer {
    void optimize(SymbolTable& st) const override;
	~ConstFoldingOptimizer() override = default;
};

} // namespace cmp
