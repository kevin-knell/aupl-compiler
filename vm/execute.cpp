#include "value.hpp"
#include "instructions.hpp"
#include "vec2.hpp"
#include "vm.hpp"
#include <cstring>

namespace vm {
    void run_vm(vm::VirtualMachine& vm) {
        constexpr size_t STACK_SIZE = 1024;

        Instruction*& code = vm.code;
        Instruction* ip = code + vm.main_start;

        //Value* static_memory = new Value[5];
        //Value* const_memory = vm.const_memory;
        //(void) const_memory;
        
        Value* stack = new Value[STACK_SIZE];
        Value* sp = stack;
        Value* fp = sp;

        #define OP_PREFIX(name) &&OP_##name
        #define DECL_OP(name) [static_cast<uint8_t>(Instruction::name)] = OP_PREFIX(name)

        static void* dispatch_table[256] = {
            #define DISPATCH_ENTRY(op, size) &&OP_##op,
            OPCODES(DISPATCH_ENTRY, _)
            #undef DISPATCH_ENTRY
        };

        #define DISPATCH() goto *dispatch_table[static_cast<uint8_t>(*(ip++))]

        DISPATCH();

        #define STACK_REF(TYPE, ptr) (*reinterpret_cast<TYPE*>(ptr))
        
        #define FETCH(TYPE) (*reinterpret_cast<TYPE*>((ip += sizeof(TYPE)) - sizeof(TYPE)))
        #define FETCH_ONLY(TYPE) (*reinterpret_cast<TYPE*>(ip))

        #define ADVANCE() DISPATCH()
        #define ERROR() goto OP_ERR

        OP_NOP: ADVANCE();
        
        OP_GROW: ERROR();
        OP_SHRINK: ERROR();

		#define LOAD_CONST(TYPE)																\
			do {																				\
				struct [[gnu::packed]] LoadStruct {												\
					uint16_t dest_addr;															\
					TYPE value;																	\
				};																				\
				LoadStruct& load = FETCH(LoadStruct);                                   		\
				TYPE& v = STACK_REF(TYPE, fp + load.dest_addr);									\
				v = load.value;																	\
				std::cout << "load: " << (int)load.dest_addr << " <- " << (uint64_t)v << std::endl;		\
			} while(0);

        
        OP_LOAD_CONST_1: LOAD_CONST(uint8_t); ADVANCE();
        OP_LOAD_CONST_2: LOAD_CONST(uint16_t); ADVANCE();
        OP_LOAD_CONST_4: LOAD_CONST(uint32_t); ADVANCE();
        OP_LOAD_CONST_8: LOAD_CONST(uint64_t); ADVANCE();

        OP_LOAD_CONST_16: {
            uint16_t& offset = FETCH(uint16_t);
            Value16& v = STACK_REF(Value16, fp + offset);
			Value16& v2 = FETCH(Value16);
			v.v8[0] = v2.v8[0];
			v.v8[1] = v2.v8[1];
            std::cout << "load: " << (int)offset << " <- Value16" << std::endl;
            ADVANCE();
        }

        OP_LOAD_CONST_VAR: {
            ERROR();
        }
        
        OP_COPY_1: ERROR();
        OP_COPY_2: ERROR();
        OP_COPY_4: ERROR();
        OP_COPY_8: ERROR();
        OP_COPY_16: ERROR();
        OP_COPY_VAR: ERROR();
        
        OP_COPY_FP_SP_1: ERROR();
        OP_COPY_FP_SP_2: ERROR();
        OP_COPY_FP_SP_4: ERROR();
        OP_COPY_FP_SP_8: ERROR();
        OP_COPY_FP_SP_16: ERROR();
        OP_COPY_FP_SP_VAR: ERROR();
        
        OP_COPY_SP_FP_1: ERROR();
        OP_COPY_SP_FP_2: ERROR();
        OP_COPY_SP_FP_4: ERROR();
        OP_COPY_SP_FP_8: ERROR();
        OP_COPY_SP_FP_16: ERROR();
        OP_COPY_SP_FP_VAR: ERROR();

        #define BINARY_OP(TYPE, OP)                                             \
            do {                                                                \
			struct OperandsStruct {												\
				uint16_t dest_addr;												\
				uint16_t a_addr;												\
				uint16_t b_addr;												\
			};																	\
            auto operands = FETCH(OperandsStruct);								\
			TYPE& dest = STACK_REF(TYPE, fp + operands.dest_addr);              \
            const TYPE& operand_a = STACK_REF(TYPE, fp + operands.a_addr);      \
            const TYPE& operand_b = STACK_REF(TYPE, fp + operands.b_addr);      \
            dest = operand_a OP operand_b;                                      \
            } while (0);

        #define COMPARE_OP(TYPE, OP)                                            \
            do {                                                                \
			struct [[gnu::packed]] OperandsStruct {								\
				uint16_t a_addr;												\
				uint16_t b_addr;												\
				uint32_t jump_addr;												\
			};																	\
            const auto& operands = FETCH(OperandsStruct);						\
            const TYPE& operand_a = STACK_REF(TYPE, fp + operands.a_addr);      \
            const TYPE& operand_b = STACK_REF(TYPE, fp + operands.b_addr);      \
            if (operand_a OP operand_b) ip = code + operands.jump_addr;			\
			std::cout << "jump at: " << operands.jump_addr << std::endl;		\
            } while (0);

        #define COMPARE_CONST_OP(TYPE, OP)                                      \
            do {                                                                \
			struct [[gnu::packed]] OperandsStruct {								\
				uint16_t a_addr;												\
				TYPE b;															\
				uint32_t jump_addr;												\
			};																	\
			const auto& operands = FETCH(OperandsStruct);						\
            const TYPE& operand_a = STACK_REF(TYPE, fp + operands.a_addr);		\
            const TYPE& operand_b = operands.b;                                	\
            if (operand_a OP operand_b) ip = code + operands.jump_addr;			\
			std::cout << "jump at: " << operands.jump_addr << std::endl;		\
            } while (0);
        
        OP_COPY_TO_PTR_1: ERROR();
        OP_COPY_TO_PTR_2: ERROR();
        OP_COPY_TO_PTR_4: ERROR();
        OP_COPY_TO_PTR_8: ERROR();
        OP_COPY_TO_PTR_16: ERROR();
        OP_COPY_TO_PTR_VAR: ERROR();

        OP_PTR_OF: ERROR();

        OP_ADD_I8: BINARY_OP(int8_t, +); ADVANCE();
        OP_SUB_I8: BINARY_OP(int8_t, -); ADVANCE();
        OP_MUL_I8: BINARY_OP(int8_t, *); ADVANCE();
        OP_DIV_I8: BINARY_OP(int8_t, /); ADVANCE();
        OP_DIV_U8: BINARY_OP(uint8_t, /); ADVANCE();
        OP_MOD_I8: BINARY_OP(int8_t, %); ADVANCE();
        OP_ADD_CONST_I8: BINARY_OP(int8_t, +); ADVANCE();
        OP_SUB_CONST_I8: BINARY_OP(int8_t, -); ADVANCE();
        OP_MUL_CONST_I8: BINARY_OP(int8_t, *); ADVANCE();
        OP_DIV_CONST_I8: BINARY_OP(int8_t, /); ADVANCE();
        OP_DIV_CONST_U8: BINARY_OP(uint8_t, /); ADVANCE();
        OP_MOD_CONST_I8: BINARY_OP(int8_t, %); ADVANCE();

        OP_ADD_I16: BINARY_OP(int16_t, +); ADVANCE();
        OP_SUB_I16: BINARY_OP(int16_t, -); ADVANCE();
        OP_MUL_I16: BINARY_OP(int16_t, *); ADVANCE();
        OP_DIV_I16: BINARY_OP(int16_t, /); ADVANCE();
        OP_DIV_U16: BINARY_OP(uint16_t, /); ADVANCE();
        OP_MOD_I16: BINARY_OP(int16_t, %); ADVANCE();
        OP_ADD_CONST_I16: BINARY_OP(int16_t, +); ADVANCE();
        OP_SUB_CONST_I16: BINARY_OP(int16_t, -); ADVANCE();
        OP_MUL_CONST_I16: BINARY_OP(int16_t, *); ADVANCE();
        OP_DIV_CONST_I16: BINARY_OP(int16_t, /); ADVANCE();
        OP_DIV_CONST_U16: BINARY_OP(uint16_t, /); ADVANCE();
        OP_MOD_CONST_I16: BINARY_OP(int16_t, %); ADVANCE();

        OP_ADD_I32: BINARY_OP(int32_t, +); ADVANCE();
        OP_SUB_I32: BINARY_OP(int32_t, -); ADVANCE();
        OP_MUL_I32: BINARY_OP(int32_t, *); ADVANCE();
        OP_DIV_I32: BINARY_OP(int32_t, /); ADVANCE();
        OP_DIV_U32: BINARY_OP(uint32_t, /); ADVANCE();
        OP_MOD_I32: BINARY_OP(int32_t, %); ADVANCE();
        OP_ADD_CONST_I32: BINARY_OP(int32_t, +); ADVANCE();
        OP_SUB_CONST_I32: BINARY_OP(int32_t, -); ADVANCE();
        OP_MUL_CONST_I32: BINARY_OP(int32_t, *); ADVANCE();
        OP_DIV_CONST_I32: BINARY_OP(int32_t, /); ADVANCE();
        OP_DIV_CONST_U32: BINARY_OP(uint32_t, /); ADVANCE();
        OP_MOD_CONST_I32: BINARY_OP(int32_t, %); ADVANCE();

        OP_ADD_I64: BINARY_OP(int64_t, +); ADVANCE();
        OP_SUB_I64: BINARY_OP(int64_t, -); ADVANCE();
        OP_MUL_I64: BINARY_OP(int64_t, *); ADVANCE();
        OP_DIV_I64: BINARY_OP(int64_t, /); ADVANCE();
        OP_DIV_U64: BINARY_OP(uint64_t, /); ADVANCE();
        OP_MOD_I64: BINARY_OP(int64_t, %); ADVANCE();
        OP_ADD_CONST_I64: BINARY_OP(int64_t, +); ADVANCE();
        OP_SUB_CONST_I64: BINARY_OP(int64_t, -); ADVANCE();
        OP_MUL_CONST_I64: BINARY_OP(int64_t, *); ADVANCE();
        OP_DIV_CONST_I64: BINARY_OP(int64_t, /); ADVANCE();
        OP_DIV_CONST_U64: BINARY_OP(uint64_t, /); ADVANCE();
        OP_MOD_CONST_I64: BINARY_OP(int64_t, %); ADVANCE();

        OP_ADD_FLOAT: BINARY_OP(float, +); ADVANCE();
        OP_SUB_FLOAT: BINARY_OP(float, -); ADVANCE();
        OP_MUL_FLOAT: BINARY_OP(float, *); ADVANCE();
        OP_DIV_FLOAT: BINARY_OP(float, /); ADVANCE();
        OP_ADD_CONST_FLOAT: BINARY_OP(float, +); ADVANCE();
        OP_SUB_CONST_FLOAT: BINARY_OP(float, -); ADVANCE();
        OP_MUL_CONST_FLOAT: BINARY_OP(float, *); ADVANCE();
        OP_DIV_CONST_FLOAT: BINARY_OP(float, /); ADVANCE();

        OP_ADD_DOUBLE: BINARY_OP(double, +); ADVANCE();
        OP_SUB_DOUBLE: BINARY_OP(double, -); ADVANCE();
        OP_MUL_DOUBLE: BINARY_OP(double, *); ADVANCE();
        OP_DIV_DOUBLE: BINARY_OP(double, /); ADVANCE();
        OP_ADD_CONST_DOUBLE: BINARY_OP(double, +); ADVANCE();
        OP_SUB_CONST_DOUBLE: BINARY_OP(double, -); ADVANCE();
        OP_MUL_CONST_DOUBLE: BINARY_OP(double, *); ADVANCE();
        OP_DIV_CONST_DOUBLE: BINARY_OP(double, /); ADVANCE();

        OP_IF_EQ_I8: COMPARE_OP(int8_t, ==); ADVANCE();
        OP_IF_NEQ_I8: COMPARE_OP(int8_t, !=); ADVANCE();
        OP_IF_LT_I8: COMPARE_OP(int8_t, <); ADVANCE();
        OP_IF_ELT_I8: COMPARE_OP(int8_t, <=); ADVANCE();
        OP_IF_EQ_CONST_I8: COMPARE_CONST_OP(int8_t, ==); ADVANCE();
        OP_IF_NEQ_CONST_I8: COMPARE_CONST_OP(int8_t, !=); ADVANCE();
        OP_IF_LT_CONST_I8: COMPARE_CONST_OP(int8_t, <); ADVANCE();
        OP_IF_ELT_CONST_I8: COMPARE_CONST_OP(int8_t, <=); ADVANCE();
        OP_IF_GT_CONST_I8: COMPARE_CONST_OP(int8_t, >); ADVANCE();
        OP_IF_EGT_CONST_I8: COMPARE_CONST_OP(int8_t, >=); ADVANCE();

        OP_IF_EQ_I16: COMPARE_OP(int16_t, ==); ADVANCE();
        OP_IF_NEQ_I16: COMPARE_OP(int16_t, !=); ADVANCE();
        OP_IF_LT_I16: COMPARE_OP(int16_t, <); ADVANCE();
        OP_IF_ELT_I16: COMPARE_OP(int16_t, <=); ADVANCE();
        OP_IF_EQ_CONST_I16: COMPARE_CONST_OP(int16_t, ==); ADVANCE();
        OP_IF_NEQ_CONST_I16: COMPARE_CONST_OP(int16_t, !=); ADVANCE();
        OP_IF_LT_CONST_I16: COMPARE_CONST_OP(int16_t, <); ADVANCE();
        OP_IF_ELT_CONST_I16: COMPARE_CONST_OP(int16_t, <=); ADVANCE();
        OP_IF_GT_CONST_I16: COMPARE_CONST_OP(int16_t, >); ADVANCE();
        OP_IF_EGT_CONST_I16: COMPARE_CONST_OP(int16_t, >=); ADVANCE();

        OP_IF_EQ_I32: COMPARE_OP(int32_t, ==); ADVANCE();
        OP_IF_NEQ_I32: COMPARE_OP(int32_t, !=); ADVANCE();
        OP_IF_LT_I32: COMPARE_OP(int32_t, <); ADVANCE();
        OP_IF_ELT_I32: COMPARE_OP(int32_t, <=); ADVANCE();
        OP_IF_EQ_CONST_I32: COMPARE_CONST_OP(int32_t, ==); ADVANCE();
        OP_IF_NEQ_CONST_I32: COMPARE_CONST_OP(int32_t, !=); ADVANCE();
        OP_IF_LT_CONST_I32: COMPARE_CONST_OP(int32_t, <); ADVANCE();
        OP_IF_ELT_CONST_I32: COMPARE_CONST_OP(int32_t, <=); ADVANCE();
        OP_IF_GT_CONST_I32: COMPARE_CONST_OP(int32_t, >); ADVANCE();
        OP_IF_EGT_CONST_I32: COMPARE_CONST_OP(int32_t, >=); ADVANCE();

        OP_IF_EQ_I64: COMPARE_OP(int64_t, ==); ADVANCE();
        OP_IF_NEQ_I64: COMPARE_OP(int64_t, !=); ADVANCE();
        OP_IF_LT_I64: COMPARE_OP(int64_t, <); ADVANCE();
        OP_IF_ELT_I64: COMPARE_OP(int64_t, <=); ADVANCE();
        OP_IF_EQ_CONST_I64: COMPARE_CONST_OP(int64_t, ==); ADVANCE();
        OP_IF_NEQ_CONST_I64: COMPARE_CONST_OP(int64_t, !=); ADVANCE();
        OP_IF_LT_CONST_I64: COMPARE_CONST_OP(int64_t, <); ADVANCE();
        OP_IF_ELT_CONST_I64: COMPARE_CONST_OP(int64_t, <=); ADVANCE();
        OP_IF_GT_CONST_I64: COMPARE_CONST_OP(int64_t, >); ADVANCE();
        OP_IF_EGT_CONST_I64: COMPARE_CONST_OP(int64_t, >=); ADVANCE();

        OP_IF_EQ_U8: COMPARE_OP(uint8_t, ==); ADVANCE();
        OP_IF_NEQ_U8: COMPARE_OP(uint8_t, !=); ADVANCE();
        OP_IF_LT_U8: COMPARE_OP(uint8_t, <); ADVANCE();
        OP_IF_ELT_U8: COMPARE_OP(uint8_t, <=); ADVANCE();
        OP_IF_EQ_CONST_U8: COMPARE_CONST_OP(uint8_t, ==); ADVANCE();
        OP_IF_NEQ_CONST_U8: COMPARE_CONST_OP(uint8_t, !=); ADVANCE();
        OP_IF_LT_CONST_U8: COMPARE_CONST_OP(uint8_t, <); ADVANCE();
        OP_IF_ELT_CONST_U8: COMPARE_CONST_OP(uint8_t, <=); ADVANCE();
        OP_IF_GT_CONST_U8: COMPARE_CONST_OP(uint8_t, >); ADVANCE();
        OP_IF_EGT_CONST_U8: COMPARE_CONST_OP(uint8_t, >=); ADVANCE();

        OP_IF_EQ_U16: COMPARE_OP(uint16_t, ==); ADVANCE();
        OP_IF_NEQ_U16: COMPARE_OP(uint16_t, !=); ADVANCE();
        OP_IF_LT_U16: COMPARE_OP(uint16_t, <); ADVANCE();
        OP_IF_ELT_U16: COMPARE_OP(uint16_t, <=); ADVANCE();
        OP_IF_EQ_CONST_U16: COMPARE_CONST_OP(uint16_t, ==); ADVANCE();
        OP_IF_NEQ_CONST_U16: COMPARE_CONST_OP(uint16_t, !=); ADVANCE();
        OP_IF_LT_CONST_U16: COMPARE_CONST_OP(uint16_t, <); ADVANCE();
        OP_IF_ELT_CONST_U16: COMPARE_CONST_OP(uint16_t, <=); ADVANCE();
        OP_IF_GT_CONST_U16: COMPARE_CONST_OP(uint16_t, >); ADVANCE();
        OP_IF_EGT_CONST_U16: COMPARE_CONST_OP(uint16_t, >=); ADVANCE();

        OP_IF_EQ_U32: COMPARE_OP(uint32_t, ==); ADVANCE();
        OP_IF_NEQ_U32: COMPARE_OP(uint32_t, !=); ADVANCE();
        OP_IF_LT_U32: COMPARE_OP(uint32_t, <); ADVANCE();
        OP_IF_ELT_U32: COMPARE_OP(uint32_t, <=); ADVANCE();
        OP_IF_EQ_CONST_U32: COMPARE_CONST_OP(uint32_t, ==); ADVANCE();
        OP_IF_NEQ_CONST_U32: COMPARE_CONST_OP(uint32_t, !=); ADVANCE();
        OP_IF_LT_CONST_U32: COMPARE_CONST_OP(uint32_t, <); ADVANCE();
        OP_IF_ELT_CONST_U32: COMPARE_CONST_OP(uint32_t, <=); ADVANCE();
        OP_IF_GT_CONST_U32: COMPARE_CONST_OP(uint32_t, >); ADVANCE();
        OP_IF_EGT_CONST_U32: COMPARE_CONST_OP(uint32_t, >=); ADVANCE();

        OP_IF_EQ_U64: COMPARE_OP(uint64_t, ==); ADVANCE();
        OP_IF_NEQ_U64: COMPARE_OP(uint64_t, !=); ADVANCE();
        OP_IF_LT_U64: COMPARE_OP(uint64_t, <); ADVANCE();
        OP_IF_ELT_U64: COMPARE_OP(uint64_t, <=); ADVANCE();
        OP_IF_EQ_CONST_U64: COMPARE_CONST_OP(uint64_t, ==); ADVANCE();
        OP_IF_NEQ_CONST_U64: COMPARE_CONST_OP(uint64_t, !=); ADVANCE();
        OP_IF_LT_CONST_U64: COMPARE_CONST_OP(uint64_t, <); ADVANCE();
        OP_IF_ELT_CONST_U64: COMPARE_CONST_OP(uint64_t, <=); ADVANCE();
        OP_IF_GT_CONST_U64: COMPARE_CONST_OP(uint64_t, >); ADVANCE();
        OP_IF_EGT_CONST_U64: COMPARE_CONST_OP(uint64_t, >=); ADVANCE();

        OP_IF_EQ_FLOAT: COMPARE_OP(float, ==); ADVANCE();
        OP_IF_NEQ_FLOAT: COMPARE_OP(float, !=); ADVANCE();
        OP_IF_LT_FLOAT: COMPARE_OP(float, <); ADVANCE();
        OP_IF_ELT_FLOAT: COMPARE_OP(float, <=); ADVANCE();
        OP_IF_EQ_CONST_FLOAT: COMPARE_CONST_OP(float, ==); ADVANCE();
        OP_IF_NEQ_CONST_FLOAT: COMPARE_CONST_OP(float, !=); ADVANCE();
        OP_IF_LT_CONST_FLOAT: COMPARE_CONST_OP(float, <); ADVANCE();
        OP_IF_ELT_CONST_FLOAT: COMPARE_CONST_OP(float, <=); ADVANCE();
        OP_IF_GT_CONST_FLOAT: COMPARE_CONST_OP(float, >); ADVANCE();
        OP_IF_EGT_CONST_FLOAT: COMPARE_CONST_OP(float, >=); ADVANCE();

        OP_IF_EQ_DOUBLE: COMPARE_OP(double, ==); ADVANCE();
        OP_IF_NEQ_DOUBLE: COMPARE_OP(double, !=); ADVANCE();
        OP_IF_LT_DOUBLE: COMPARE_OP(double, <); ADVANCE();
        OP_IF_ELT_DOUBLE: COMPARE_OP(double, <=); ADVANCE();
        OP_IF_EQ_CONST_DOUBLE: COMPARE_CONST_OP(double, ==); ADVANCE();
        OP_IF_NEQ_CONST_DOUBLE: COMPARE_CONST_OP(double, !=); ADVANCE();
        OP_IF_LT_CONST_DOUBLE: COMPARE_CONST_OP(double, <); ADVANCE();
        OP_IF_ELT_CONST_DOUBLE: COMPARE_CONST_OP(double, <=); ADVANCE();
        OP_IF_GT_CONST_DOUBLE: COMPARE_CONST_OP(double, >); ADVANCE();
        OP_IF_EGT_CONST_DOUBLE: COMPARE_CONST_OP(double, >=); ADVANCE();

        OP_CALL_1: {
            Instruction*& func_addr = FETCH(Instruction*);

            Instruction*& return_address = STACK_REF(Instruction*, sp);
            return_address = ip;
            
            ip = func_addr;

            Value*& last_frame = STACK_REF(Value*, sp);
            last_frame = fp;
            
            fp = sp;

            DISPATCH();
        }

		OP_CALL_2: ERROR();
		OP_CALL_3: ERROR();
		OP_CALL_4: ERROR();
		OP_CALL_VAR: ERROR();

        OP_RET: {
            ERROR();
        }

        OP_CALL_NATIVE: {
            uint16_t class_id = FETCH(uint16_t);
            uint16_t method_id = FETCH(uint16_t);
            void* obj = STACK_REF(void*, fp + FETCH(uint16_t));
            vm.db->classes[class_id].methods[method_id].value_call(sp, obj);
            ADVANCE();
        }

        OP_GOTO: {
            ip = code + FETCH(uint32_t);
            DISPATCH();
        }

        OP_IF_FALSE_GOTO: {
            DISPATCH();
        }

        OP_PRINT: ERROR();
        
        OP_ALT: ERROR();

        OP_DIRECT_THREADED: ERROR();

        OP_ERR: {
            std::cout << "runtime error" << std::endl;
            return;
        }

        OP_HALT: {
            std::cout << "halt" << std::endl;
            return;
        }

        #undef DISPATCH
    }
}