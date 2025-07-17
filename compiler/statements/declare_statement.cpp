#include "declare_statement.hpp"
#include "instructions.hpp"
#include "primitive_type.hpp"
#include "value.hpp"
#include "binary_op_expression.hpp"
#include "variable_expression.hpp"
#include "load_const_expression.hpp"
#include "color.hpp"

#define TAG(s, b) std::string(b ? s : "")

namespace cmp {

DeclareStatement::DeclareStatement(const VarPtr& variable_symbol)
    : variable_symbol(variable_symbol) {}

std::string DeclareStatement::to_string() const {
    return TAG(C_KEYWORD("volatile "), is_volatile) + variable_symbol->to_string() + " " +
           (variable_symbol->initial_value != nullptr
               ? "= " + variable_symbol->initial_value->to_string()
               : "") + " ";
}

namespace {
    std::vector<uint8_t> generate_binary_bytecode(const BinaryExpression& binary_expr, const VarPtr& variable_symbol, BytecodeGenerationInfo& bgi) {
        auto left_op = std::dynamic_pointer_cast<VariableExpression>(binary_expr.left);
        auto right_op = std::dynamic_pointer_cast<VariableExpression>(binary_expr.right);
        vm::Value2 dest{ .u16 = static_cast<uint16_t>(bgi.scope->variable_indices[variable_symbol->name]) };
        vm::Value2 left{ .u16 = static_cast<uint16_t>(bgi.scope->variable_indices[left_op->var->name]) };
        vm::Value2 right{ .u16 = static_cast<uint16_t>(bgi.scope->variable_indices[right_op->var->name]) };
		uint8_t op_code = 0;

		//bool has_const = left_op->var->initial_value->is_constexpr() || right_op->var->initial_value->is_constexpr();

		vm::BinType bin_type;

		if (variable_symbol->type == PrimitiveType::TYPE_U8) {
			bin_type = vm::BinType::UINT8;
		} else if (variable_symbol->type == PrimitiveType::TYPE_I8) {
			bin_type = vm::BinType::INT8;
		} else if (variable_symbol->type == PrimitiveType::TYPE_U16) {
			bin_type = vm::BinType::UINT16;
		} else if (variable_symbol->type == PrimitiveType::TYPE_I16) {
			bin_type = vm::BinType::INT16;
		} else if (variable_symbol->type == PrimitiveType::TYPE_U32) {
			bin_type = vm::BinType::UINT32;
		} else if (variable_symbol->type == PrimitiveType::TYPE_I32) {
			bin_type = vm::BinType::INT32;
		} else if (variable_symbol->type == PrimitiveType::TYPE_U64) {
			bin_type = vm::BinType::UINT64;
		} else if (variable_symbol->type == PrimitiveType::TYPE_INT) {
			bin_type = vm::BinType::INT64;
		} else if (variable_symbol->type == PrimitiveType::TYPE_F32) {
			bin_type = vm::BinType::FLOAT;
		} else if (variable_symbol->type == PrimitiveType::TYPE_FLOAT) {
			bin_type = vm::BinType::DOUBLE;
		} else {
			std::cerr << "Unsupported type for binary operation: " << variable_symbol->type->to_string() << std::endl;
			return { static_cast<uint8_t>(vm::Instruction::ERR) };
		}

		op_code = static_cast<uint8_t>(vm::get_binary_opcode(
			bin_type,
			binary_expr.op,
			false //has_const
		));

		return {
			op_code,
			dest.v[0].u8, dest.v[1].u8,
			left.v[0].u8, left.v[1].u8,
			right.v[0].u8, right.v[1].u8,
		};
    }

    std::vector<uint8_t> generate_load_const_bytecode(const ExprPtr& value, const VarPtr& variable_symbol, BytecodeGenerationInfo& bgi) {
        std::vector<uint8_t> result;
        int size = variable_symbol->type->get_size();
        if (size == 0) return {};
		
        vm::Value* v = value->eval_constexpr();
        vm::Value2 dest{ .u16 = static_cast<uint16_t>(bgi.scope->variable_indices[variable_symbol->name]) };
        switch (size) {
            case 1: result.push_back((uint8_t)vm::Instruction::LOAD_CONST_1); break;
            case 2: result.push_back((uint8_t)vm::Instruction::LOAD_CONST_2); break;
            case 4: result.push_back((uint8_t)vm::Instruction::LOAD_CONST_4); break;
            case 8: result.push_back((uint8_t)vm::Instruction::LOAD_CONST_8); break;
            case 16: result.push_back((uint8_t)vm::Instruction::LOAD_CONST_16); break;
            default:
                result.push_back((uint8_t)vm::Instruction::LOAD_CONST_VAR);
                result.push_back(size);
        }
        result.push_back(dest.v[0].u8);
        result.push_back(dest.v[1].u8);
        for (int i = 0; i < size; ++i) {
            result.push_back(v[i].u8);
        }
        return result;
    }
}

std::vector<uint8_t> DeclareStatement::generate_bytecode(BytecodeGenerationInfo& bgi) const {
    ExprPtr value = variable_symbol->initial_value;
    if (!value) return {};

    switch(value->get_kind()) {
        case Expression::BINARY: {
            auto binary_expr = std::dynamic_pointer_cast<BinaryExpression>(value);
            if (binary_expr)
                return generate_binary_bytecode(*binary_expr, variable_symbol, bgi);
            else
                return { static_cast<uint8_t>(vm::Instruction::ERR) };
        }
        case Expression::LOAD_CONST:
            return generate_load_const_bytecode(value, variable_symbol, bgi);
        case Expression::CALL:
            return { static_cast<uint8_t>(vm::Instruction::ERR) };
        default:
            return { static_cast<uint8_t>(vm::Instruction::ERR) };
    }
}

size_t DeclareStatement::get_bytecode_size(BytecodeGenerationInfo &bgi) const {
    ExprPtr value = variable_symbol->initial_value;
    if (!value) return 0;
    
    switch(value->get_kind()) {
        case Expression::BINARY:
            return 1 + 2 + 2 + 2;
            //else return 1;
        case Expression::LOAD_CONST: {
            int size = variable_symbol->type->get_size();
            switch (size) {
                case 0: return 0;
                case 1: return 1 + 2 + 1;
                case 2: return 1 + 2 + 2;
                case 4: return 1 + 2 + 4;
                case 8: return 1 + 2 + 8;
                case 16: return 1 + 2 + 16;
                default: return 1 + 1 + 2 + size;
            }
        }
        default: return 1; // ERR
    }
}

std::vector<ExprPtr*> DeclareStatement::get_expressions() {
    if (variable_symbol->initial_value) return {&variable_symbol->initial_value};
    else return {};
}

} // namespace cmp

#undef TAG