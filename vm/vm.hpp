#pragma once
#include <vector>
#include "class.hpp"
#include "function.hpp"
#include "value.hpp"
#include "class_db.hpp"
#include "instructions.hpp"

namespace vm {
    struct VirtualMachine {
        const ClassDB& db;

        Instruction* code;
        Value* const_memory;
        
		size_t main_start;

		VirtualMachine(const ClassDB& db)
			: db(db) {}
    };
}