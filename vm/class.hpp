#pragma once
#include <vector>
#include <cstddef>
#include "function.hpp"

namespace vm {
    struct VMClass {
        size_t memory_size;
        BytecodeFunction* functions;
    };
}