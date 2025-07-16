#pragma once

#include "class_symbol.hpp"
#include <unordered_map>
#include <string>

namespace cmp {

struct SymbolTable {
    std::unordered_map<std::string, ClassPtr> classes;
};

}