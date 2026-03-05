#pragma once
#include <memory>
#include "forward_declarations.hpp"

namespace cmp {

    struct NameAnalysisInfo {
        SymbolTable& symbol_table;
        ClassPtr cls;
        FuncPtr f;
		ScopePtr scope;
    };
} // namespace cmp
