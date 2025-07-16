#pragma once
#include <cstdint>
#include "instructions.hpp"

namespace vm {
    class Value;

    struct BytecodeFunction {
        size_t arg_size;
        Value* initial_memory;
        size_t frame_size;
        Instruction* code_start;
    };
}