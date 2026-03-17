#pragma once

#include <iostream>
#include <vector>

#define OPCODES(_, s) \
    _(NOP, s(1)) \
    \
    _(GROW, s(1, 2)) \
    _(SHRINK, s(1, 2)) \
    \
    _(LOAD_CONST_1, s(1, 2, 1)) \
    _(LOAD_CONST_2, s(1, 2, 2)) \
    _(LOAD_CONST_4, s(1, 2, 4)) \
    _(LOAD_CONST_8, s(1, 2, 8)) \
    _(LOAD_CONST_16, s(1, 2, 16)) \
    _(LOAD_CONST_VAR, s(1, 2, 2)) \
	\
	_(LOAD_STRING, s(1, 2, 2)) \
    \
    _(COPY_1, s(1, 2, 2)) \
    _(COPY_2, s(1, 2, 2)) \
    _(COPY_4, s(1, 2, 2)) \
    _(COPY_8, s(1, 2, 2)) \
    _(COPY_16, s(1, 2, 2)) \
    _(COPY_VAR, s(1, 2, 2, 2)) \
    \
    _(COPY_FP_SP_1, s(1, 2, 2)) \
    _(COPY_FP_SP_2, s(1, 2, 2)) \
    _(COPY_FP_SP_4, s(1, 2, 2)) \
    _(COPY_FP_SP_8, s(1, 2, 2)) \
    _(COPY_FP_SP_16, s(1, 2, 2)) \
    _(COPY_FP_SP_VAR, s(1, 2, 2, 2)) \
    \
    _(COPY_SP_FP_1, s(1, 2, 2)) \
    _(COPY_SP_FP_2, s(1, 2, 2)) \
    _(COPY_SP_FP_4, s(1, 2, 2)) \
    _(COPY_SP_FP_8, s(1, 2, 2)) \
    _(COPY_SP_FP_16, s(1, 2, 2)) \
    _(COPY_SP_FP_VAR, s(1, 2, 2, 2)) \
    \
    _(COPY_TO_PTR_1, s(1, 2, 2)) \
    _(COPY_TO_PTR_2, s(1, 2, 2)) \
    _(COPY_TO_PTR_4, s(1, 2, 2)) \
    _(COPY_TO_PTR_8, s(1, 2, 2)) \
    _(COPY_TO_PTR_16, s(1, 2, 2)) \
    _(COPY_TO_PTR_VAR, s(1, 2, 2, 2)) \
    \
    _(PTR_OF, s(1, 2, 2)) \
    \
    _(COPY_TO_STATIC_1, s(1, 2, 2)) \
    _(COPY_TO_STATIC_2, s(1, 2, 2)) \
    _(COPY_TO_STATIC_4, s(1, 2, 2)) \
    _(COPY_TO_STATIC_8, s(1, 2, 2)) \
    _(COPY_TO_STATIC_16, s(1, 2, 2)) \
    _(COPY_TO_STATIC_VAR, s(1, 2, 2, 2)) \
    \
    _(ADD_I8, s(1, 2, 2, 2)) \
    _(SUB_I8, s(1, 2, 2, 2)) \
    _(MUL_I8, s(1, 2, 2, 2)) \
    _(DIV_I8, s(1, 2, 2, 2)) \
    _(DIV_U8, s(1, 2, 2, 2)) \
    _(MOD_I8, s(1, 2, 2, 2)) \
    _(ADD_CONST_I8, s(1, 2, 2, 1)) \
    _(SUB_CONST_I8, s(1, 2, 2, 1)) \
    _(MUL_CONST_I8, s(1, 2, 2, 1)) \
    _(DIV_CONST_I8, s(1, 2, 2, 1)) \
    _(DIV_CONST_U8, s(1, 2, 2, 1)) \
    _(MOD_CONST_I8, s(1, 2, 2, 1)) \
    \
    _(ADD_I16, s(1, 2, 2, 2)) \
    _(SUB_I16, s(1, 2, 2, 2)) \
    _(MUL_I16, s(1, 2, 2, 2)) \
    _(DIV_I16, s(1, 2, 2, 2)) \
    _(DIV_U16, s(1, 2, 2, 2)) \
    _(MOD_I16, s(1, 2, 2, 2)) \
    _(ADD_CONST_I16, s(1, 2, 2, 2)) \
    _(SUB_CONST_I16, s(1, 2, 2, 2)) \
    _(MUL_CONST_I16, s(1, 2, 2, 2)) \
    _(DIV_CONST_I16, s(1, 2, 2, 2)) \
    _(DIV_CONST_U16, s(1, 2, 2, 2)) \
    _(MOD_CONST_I16, s(1, 2, 2, 2)) \
    \
    _(ADD_I32, s(1, 2, 2, 2)) \
    _(SUB_I32, s(1, 2, 2, 2)) \
    _(MUL_I32, s(1, 2, 2, 2)) \
    _(DIV_I32, s(1, 2, 2, 2)) \
    _(DIV_U32, s(1, 2, 2, 2)) \
    _(MOD_I32, s(1, 2, 2, 2)) \
    _(ADD_CONST_I32, s(1, 2, 2, 4)) \
    _(SUB_CONST_I32, s(1, 2, 2, 4)) \
    _(MUL_CONST_I32, s(1, 2, 2, 4)) \
    _(DIV_CONST_I32, s(1, 2, 2, 4)) \
    _(DIV_CONST_U32, s(1, 2, 2, 4)) \
    _(MOD_CONST_I32, s(1, 2, 2, 4)) \
    \
    _(ADD_I64, s(1, 2, 2, 2)) \
    _(SUB_I64, s(1, 2, 2, 2)) \
    _(MUL_I64, s(1, 2, 2, 2)) \
    _(DIV_I64, s(1, 2, 2, 2)) \
    _(DIV_U64, s(1, 2, 2, 2)) \
    _(MOD_I64, s(1, 2, 2, 2)) \
    _(ADD_CONST_I64, s(1, 2, 2, 4)) \
    _(SUB_CONST_I64, s(1, 2, 2, 4)) \
    _(MUL_CONST_I64, s(1, 2, 2, 4)) \
    _(DIV_CONST_I64, s(1, 2, 2, 4)) \
    _(DIV_CONST_U64, s(1, 2, 2, 4)) \
    _(MOD_CONST_I64, s(1, 2, 2, 4)) \
    \
    _(ADD_FLOAT, s(1, 2, 2, 2)) \
    _(SUB_FLOAT, s(1, 2, 2, 2)) \
    _(MUL_FLOAT, s(1, 2, 2, 2)) \
    _(DIV_FLOAT, s(1, 2, 2, 2)) \
    _(ADD_CONST_FLOAT, s(1, 2, 2, 4)) \
    _(SUB_CONST_FLOAT, s(1, 2, 2, 4)) \
    _(MUL_CONST_FLOAT, s(1, 2, 2, 4)) \
    _(DIV_CONST_FLOAT, s(1, 2, 2, 4)) \
    \
    _(ADD_DOUBLE, s(1, 2, 2, 2)) \
    _(SUB_DOUBLE, s(1, 2, 2, 2)) \
    _(MUL_DOUBLE, s(1, 2, 2, 2)) \
    _(DIV_DOUBLE, s(1, 2, 2, 2)) \
    _(ADD_CONST_DOUBLE, s(1, 2, 2, 8)) \
    _(SUB_CONST_DOUBLE, s(1, 2, 2, 8)) \
    _(MUL_CONST_DOUBLE, s(1, 2, 2, 8)) \
    _(DIV_CONST_DOUBLE, s(1, 2, 2, 8)) \
    \
    _(IF_EQ_I8, s(1, 2, 2, 4)) \
    _(IF_NEQ_I8, s(1, 2, 2, 4)) \
    _(IF_LT_I8, s(1, 2, 2, 4)) \
    _(IF_ELT_I8, s(1, 2, 2, 4)) \
    _(IF_EQ_CONST_I8, s(1, 2, 1, 4)) \
    _(IF_NEQ_CONST_I8, s(1, 2, 1, 4)) \
    _(IF_LT_CONST_I8, s(1, 2, 1, 4)) \
    _(IF_ELT_CONST_I8, s(1, 2, 1, 4)) \
    _(IF_GT_CONST_I8, s(1, 2, 1, 4)) \
    _(IF_EGT_CONST_I8, s(1, 2, 1, 4)) \
    \
    _(IF_EQ_I16, s(1, 2, 2, 4)) \
    _(IF_NEQ_I16, s(1, 2, 2, 4)) \
    _(IF_LT_I16, s(1, 2, 2, 4)) \
    _(IF_ELT_I16, s(1, 2, 2, 4)) \
    _(IF_EQ_CONST_I16, s(1, 2, 2, 4)) \
    _(IF_NEQ_CONST_I16, s(1, 2, 2, 4)) \
    _(IF_LT_CONST_I16, s(1, 2, 2, 4)) \
    _(IF_ELT_CONST_I16, s(1, 2, 2, 4)) \
    _(IF_GT_CONST_I16, s(1, 2, 2, 4)) \
    _(IF_EGT_CONST_I16, s(1, 2, 2, 4)) \
    \
    _(IF_EQ_I32, s(1, 2, 2, 4)) \
    _(IF_NEQ_I32, s(1, 2, 2, 4)) \
    _(IF_LT_I32, s(1, 2, 2, 4)) \
    _(IF_ELT_I32, s(1, 2, 2, 4)) \
    _(IF_EQ_CONST_I32, s(1, 2, 2, 4)) \
    _(IF_NEQ_CONST_I32, s(1, 2, 2, 4)) \
    _(IF_LT_CONST_I32, s(1, 2, 2, 4)) \
    _(IF_ELT_CONST_I32, s(1, 2, 2, 4)) \
    _(IF_GT_CONST_I32, s(1, 2, 2, 4)) \
    _(IF_EGT_CONST_I32, s(1, 2, 2, 4)) \
    \
    _(IF_EQ_I64, s(1, 2, 2, 4)) \
    _(IF_NEQ_I64, s(1, 2, 2, 4)) \
    _(IF_LT_I64, s(1, 2, 2, 4)) \
    _(IF_ELT_I64, s(1, 2, 2, 4)) \
    _(IF_EQ_CONST_I64, s(1, 2, 2, 4)) \
    _(IF_NEQ_CONST_I64, s(1, 2, 2, 4)) \
    _(IF_LT_CONST_I64, s(1, 2, 2, 4)) \
    _(IF_ELT_CONST_I64, s(1, 2, 2, 4)) \
    _(IF_GT_CONST_I64, s(1, 2, 2, 4)) \
    _(IF_EGT_CONST_I64, s(1, 2, 2, 4)) \
    \
    _(IF_LT_U8, s(1, 2, 2, 4)) \
    _(IF_ELT_U8, s(1, 2, 2, 4)) \
    _(IF_LT_CONST_U8, s(1, 2, 2, 4)) \
    _(IF_ELT_CONST_U8, s(1, 2, 2, 4)) \
    _(IF_GT_CONST_U8, s(1, 2, 2, 4)) \
    _(IF_EGT_CONST_U8, s(1, 2, 2, 4)) \
    \
    _(IF_LT_U16, s(1, 2, 2, 4)) \
    _(IF_ELT_U16, s(1, 2, 2, 4)) \
    _(IF_LT_CONST_U16, s(1, 2, 2, 4)) \
    _(IF_ELT_CONST_U16, s(1, 2, 2, 4)) \
    _(IF_GT_CONST_U16, s(1, 2, 2, 4)) \
    _(IF_EGT_CONST_U16, s(1, 2, 2, 4)) \
    \
    _(IF_LT_U32, s(1, 2, 2, 4)) \
    _(IF_ELT_U32, s(1, 2, 2, 4)) \
    _(IF_LT_CONST_U32, s(1, 2, 2, 4)) \
    _(IF_ELT_CONST_U32, s(1, 2, 2, 4)) \
    _(IF_GT_CONST_U32, s(1, 2, 2, 4)) \
    _(IF_EGT_CONST_U32, s(1, 2, 2, 4)) \
    \
    _(IF_LT_U64, s(1, 2, 2, 4)) \
    _(IF_ELT_U64, s(1, 2, 2, 4)) \
    _(IF_LT_CONST_U64, s(1, 2, 2, 4)) \
    _(IF_ELT_CONST_U64, s(1, 2, 2, 4)) \
    _(IF_GT_CONST_U64, s(1, 2, 2, 4)) \
    _(IF_EGT_CONST_U64, s(1, 2, 2, 4)) \
    \
    _(IF_EQ_FLOAT, s(1, 2, 2, 4)) \
    _(IF_NEQ_FLOAT, s(1, 2, 2, 4)) \
    _(IF_LT_FLOAT, s(1, 2, 2, 4)) \
    _(IF_ELT_FLOAT, s(1, 2, 2, 4)) \
    _(IF_EQ_CONST_FLOAT, s(1, 2, 2, 4)) \
    _(IF_NEQ_CONST_FLOAT, s(1, 2, 2, 4)) \
    _(IF_LT_CONST_FLOAT, s(1, 2, 2, 4)) \
    _(IF_ELT_CONST_FLOAT, s(1, 2, 2, 4)) \
    _(IF_GT_CONST_FLOAT, s(1, 2, 2, 4)) \
    _(IF_EGT_CONST_FLOAT, s(1, 2, 2, 4)) \
    \
    _(IF_EQ_DOUBLE, s(1, 2, 2, 4)) \
    _(IF_NEQ_DOUBLE, s(1, 2, 2, 4)) \
    _(IF_LT_DOUBLE, s(1, 2, 2, 4)) \
    _(IF_ELT_DOUBLE, s(1, 2, 2, 4)) \
    _(IF_EQ_CONST_DOUBLE, s(1, 2, 2, 4)) \
    _(IF_NEQ_CONST_DOUBLE, s(1, 2, 2, 4)) \
    _(IF_LT_CONST_DOUBLE, s(1, 2, 2, 4)) \
    _(IF_ELT_CONST_DOUBLE, s(1, 2, 2, 4)) \
    _(IF_GT_CONST_DOUBLE, s(1, 2, 2, 4)) \
    _(IF_EGT_CONST_DOUBLE, s(1, 2, 2, 4)) \
    \
    _(CALL, s(1, 4, 2, 2)) \
    _(RET, s(1)) \
    \
    _(CALL_NATIVE, s(1, 2, 2, 2, 2, 2)) \
    \
    _(GOTO, s(1, 4)) \
    \
    _(ALT, s(1, 2, 2, 2)) \
    \
    _(DIRECT_THREADED, s(1)) \
    \
    _(ERR, s(1)) \
    _(HALT, s(1))


namespace vm {

    enum class BinType {
        INT8,
        UINT8,
        INT16,
        UINT16,
        INT32,
        UINT32,
        INT64,
        UINT64,
        FLOAT,
        DOUBLE
    };

    enum class BinOp {
        ADD,
        SUB,
        MUL,
        DIV,
        MOD,

        EQ,
        NEQ,
        GT,
        LT,
        GE,
        LE,

        AND,
        OR,

		NONE
    };
	
    enum class Instruction : uint8_t {
        #define ENUM_ENTRY(op, size) op,
        OPCODES(ENUM_ENTRY, _)
        #undef ENUM_ENTRY
    };

    static_assert((uint8_t)Instruction::HALT <= 255);

    #define OPCODE_SIZE(op, size) v.push_back(std::vector<int>(size));
    #define OP_SIZE_LIST(...) {__VA_ARGS__}

    const std::vector<std::vector<int>> OP_SIZES = []{
        std::vector<std::vector<int>> v;
        OPCODES(OPCODE_SIZE, OP_SIZE_LIST)
        return v;
    }();

    #undef OPCODE_SIZE
    #undef OP_SIZE_LIST

    #define OPCODE_NAME(op, size) v.push_back(#op);

	const std::vector<std::string> OP_NAMES = []{
		std::vector<std::string> v;
		OPCODES(OPCODE_NAME, _)
		return v;
	}();

	#undef OPCODE_NAME

    inline Instruction get_binary_opcode(BinType type, BinOp op, bool is_const) {
        switch (type) {
            case BinType::INT8:
                switch (op) {
                    case BinOp::ADD: return is_const ? Instruction::ADD_CONST_I8 : Instruction::ADD_I8;
                    case BinOp::SUB: return is_const ? Instruction::SUB_CONST_I8 : Instruction::SUB_I8;
                    case BinOp::MUL: return is_const ? Instruction::MUL_CONST_I8 : Instruction::MUL_I8;
                    case BinOp::DIV: return is_const ? Instruction::DIV_CONST_I8 : Instruction::DIV_I8;
                    case BinOp::MOD: return is_const ? Instruction::MOD_CONST_I8 : Instruction::MOD_I8;
					case BinOp::EQ: return is_const ? Instruction::IF_EQ_CONST_I8 : Instruction::IF_EQ_I8;
					case BinOp::NEQ: return is_const ? Instruction::IF_NEQ_CONST_I8 : Instruction::IF_EQ_I8;
					case BinOp::GT: return is_const ? Instruction::IF_GT_CONST_I8 : Instruction::ERR;
					case BinOp::LT: return is_const ? Instruction::IF_LT_CONST_I8 : Instruction::IF_LT_I8;
					case BinOp::GE: return is_const ? Instruction::IF_EGT_CONST_I8 : Instruction::ERR;
					case BinOp::LE: return is_const ? Instruction::IF_ELT_CONST_I8 : Instruction::IF_ELT_I8;
                }
                break;
            case BinType::UINT8:
                switch (op) {
                    case BinOp::DIV: return is_const ? Instruction::DIV_CONST_U8 : Instruction::DIV_U8;
                    // Add other ops as needed
                }
                break;
            case BinType::INT16:
                switch (op) {
                    case BinOp::ADD: return is_const ? Instruction::ADD_CONST_I16 : Instruction::ADD_I16;
                    case BinOp::SUB: return is_const ? Instruction::SUB_CONST_I16 : Instruction::SUB_I16;
                    case BinOp::MUL: return is_const ? Instruction::MUL_CONST_I16 : Instruction::MUL_I16;
                    case BinOp::DIV: return is_const ? Instruction::DIV_CONST_I16 : Instruction::DIV_I16;
                    case BinOp::MOD: return is_const ? Instruction::MOD_CONST_I16 : Instruction::MOD_I16;
					case BinOp::EQ: return is_const ? Instruction::IF_EQ_CONST_I16 : Instruction::IF_EQ_I16;
					case BinOp::NEQ: return is_const ? Instruction::IF_NEQ_CONST_I16 : Instruction::IF_EQ_I16;
					case BinOp::GT: return is_const ? Instruction::IF_GT_CONST_I16 : Instruction::ERR;
					case BinOp::LT: return is_const ? Instruction::IF_LT_CONST_I16 : Instruction::IF_LT_I16;
					case BinOp::GE: return is_const ? Instruction::IF_EGT_CONST_I16 : Instruction::ERR;
					case BinOp::LE: return is_const ? Instruction::IF_ELT_CONST_I16 : Instruction::IF_ELT_I16;
                }
                break;
            case BinType::UINT16:
                switch (op) {
                    case BinOp::DIV: return is_const ? Instruction::DIV_CONST_U16 : Instruction::DIV_U16;
                    // Add other ops as needed
                }
                break;
            case BinType::INT32:
                switch (op) {
                    case BinOp::ADD: return is_const ? Instruction::ADD_CONST_I32 : Instruction::ADD_I32;
                    case BinOp::SUB: return is_const ? Instruction::SUB_CONST_I32 : Instruction::SUB_I32;
                    case BinOp::MUL: return is_const ? Instruction::MUL_CONST_I32 : Instruction::MUL_I32;
                    case BinOp::DIV: return is_const ? Instruction::DIV_CONST_I32 : Instruction::DIV_I32;
                    case BinOp::MOD: return is_const ? Instruction::MOD_CONST_I32 : Instruction::MOD_I32;
					case BinOp::EQ: return is_const ? Instruction::IF_EQ_CONST_I32 : Instruction::IF_EQ_I32;
					case BinOp::NEQ: return is_const ? Instruction::IF_NEQ_CONST_I32 : Instruction::IF_EQ_I32;
					case BinOp::GT: return is_const ? Instruction::IF_GT_CONST_I32 : Instruction::ERR;
					case BinOp::LT: return is_const ? Instruction::IF_LT_CONST_I32 : Instruction::IF_LT_I32;
					case BinOp::GE: return is_const ? Instruction::IF_EGT_CONST_I32 : Instruction::ERR;
					case BinOp::LE: return is_const ? Instruction::IF_ELT_CONST_I32 : Instruction::IF_ELT_I32;
                }
                break;
            case BinType::UINT32:
                switch (op) {
                    case BinOp::DIV: return is_const ? Instruction::DIV_CONST_U32 : Instruction::DIV_U32;
                    // Add other ops as needed
                }
                break;
            case BinType::INT64:
                switch (op) {
                    case BinOp::ADD: return is_const ? Instruction::ADD_CONST_I64 : Instruction::ADD_I64;
                    case BinOp::SUB: return is_const ? Instruction::SUB_CONST_I64 : Instruction::SUB_I64;
                    case BinOp::MUL: return is_const ? Instruction::MUL_CONST_I64 : Instruction::MUL_I64;
                    case BinOp::DIV: return is_const ? Instruction::DIV_CONST_I64 : Instruction::DIV_I64;
                    case BinOp::MOD: return is_const ? Instruction::MOD_CONST_I64 : Instruction::MOD_I64;
					case BinOp::EQ: return is_const ? Instruction::IF_EQ_CONST_I64 : Instruction::IF_EQ_I64;
					case BinOp::NEQ: return is_const ? Instruction::IF_NEQ_CONST_I64 : Instruction::IF_EQ_I64;
					case BinOp::GT: return is_const ? Instruction::IF_GT_CONST_I64 : Instruction::ERR;
					case BinOp::LT: return is_const ? Instruction::IF_LT_CONST_I64 : Instruction::IF_LT_I64;
					case BinOp::GE: return is_const ? Instruction::IF_EGT_CONST_I64 : Instruction::ERR;
					case BinOp::LE: return is_const ? Instruction::IF_ELT_CONST_I64 : Instruction::IF_ELT_I64;
                }
                break;
            case BinType::UINT64:
                switch (op) {
                    case BinOp::DIV: return is_const ? Instruction::DIV_CONST_U64 : Instruction::DIV_U64;
                    // Add other ops as needed
                }
                break;
            case BinType::FLOAT:
                switch (op) {
                    case BinOp::ADD: return is_const ? Instruction::ADD_CONST_FLOAT : Instruction::ADD_FLOAT;
                    case BinOp::SUB: return is_const ? Instruction::SUB_CONST_FLOAT : Instruction::SUB_FLOAT;
                    case BinOp::MUL: return is_const ? Instruction::MUL_CONST_FLOAT : Instruction::MUL_FLOAT;
                    case BinOp::DIV: return is_const ? Instruction::DIV_CONST_FLOAT : Instruction::DIV_FLOAT;
                    // No MOD for float
					case BinOp::EQ: return is_const ? Instruction::IF_EQ_CONST_FLOAT : Instruction::IF_EQ_FLOAT;
					case BinOp::NEQ: return is_const ? Instruction::IF_NEQ_CONST_FLOAT : Instruction::IF_EQ_FLOAT;
					case BinOp::GT: return is_const ? Instruction::IF_GT_CONST_FLOAT : Instruction::ERR;
					case BinOp::LT: return is_const ? Instruction::IF_LT_CONST_FLOAT : Instruction::IF_LT_FLOAT;
					case BinOp::GE: return is_const ? Instruction::IF_EGT_CONST_FLOAT : Instruction::ERR;
					case BinOp::LE: return is_const ? Instruction::IF_ELT_CONST_FLOAT : Instruction::IF_ELT_FLOAT;
                }
                break;
            case BinType::DOUBLE:
                switch (op) {
                    case BinOp::ADD: return is_const ? Instruction::ADD_CONST_DOUBLE : Instruction::ADD_DOUBLE;
                    case BinOp::SUB: return is_const ? Instruction::SUB_CONST_DOUBLE : Instruction::SUB_DOUBLE;
                    case BinOp::MUL: return is_const ? Instruction::MUL_CONST_DOUBLE : Instruction::MUL_DOUBLE;
                    case BinOp::DIV: return is_const ? Instruction::DIV_CONST_DOUBLE : Instruction::DIV_DOUBLE;
                    // No MOD for double
					case BinOp::EQ: return is_const ? Instruction::IF_EQ_CONST_DOUBLE : Instruction::IF_EQ_DOUBLE;
					case BinOp::NEQ: return is_const ? Instruction::IF_NEQ_CONST_DOUBLE : Instruction::IF_EQ_DOUBLE;
					case BinOp::GT: return is_const ? Instruction::IF_GT_CONST_DOUBLE : Instruction::ERR;
					case BinOp::LT: return is_const ? Instruction::IF_LT_CONST_DOUBLE : Instruction::IF_LT_DOUBLE;
					case BinOp::GE: return is_const ? Instruction::IF_EGT_CONST_DOUBLE : Instruction::ERR;
					case BinOp::LE: return is_const ? Instruction::IF_ELT_CONST_DOUBLE : Instruction::IF_ELT_DOUBLE;
                }
                break;
        }
        return Instruction::ERR;
    }
} // namespace vm

