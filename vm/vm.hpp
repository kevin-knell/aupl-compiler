#pragma once
#include <vector>
#include "class.hpp"
#include "function.hpp"
#include "value.hpp"
#include "class_db.hpp"
#include "instructions.hpp"

namespace vm {
    struct VirtualMachine {
        Instruction* code;
        size_t main_start;

        Value* const_memory;
        Value* static_memory;
        Value* stack;
        bool finished = false;
        
        std::vector<VMClass> user_classes;
        ClassDB* db;
    };
}