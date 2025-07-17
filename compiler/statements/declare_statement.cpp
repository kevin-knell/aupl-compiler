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

std::vector<uint8_t> DeclareStatement::generate_bytecode(BytecodeGenerationInfo& bgi) const {
    ExprPtr value = variable_symbol->initial_value;
	if (!value) return {};

    vm::Value2 dest{ .u16 = static_cast<uint16_t>(bgi.scope->variable_indices[variable_symbol->name]) };

    switch(value->get_kind()) {
        case Expression::BINARY: {
            auto binary_expr = std::dynamic_pointer_cast<BinaryExpression>(value);
            if (value->get_type() == PrimitiveType::TYPE_INT) {
                auto left_op = std::dynamic_pointer_cast<VariableExpression>(binary_expr->left);
                auto right_op = std::dynamic_pointer_cast<VariableExpression>(binary_expr->right);

                vm::Value2 left{ .u16 = static_cast<uint16_t>(bgi.scope->variable_indices[left_op->var->name]) };
                vm::Value2 right{ .u16 = static_cast<uint16_t>(bgi.scope->variable_indices[right_op->var->name]) };

                return {
                    static_cast<uint8_t>(vm::Instruction::ADD_I64),
                    dest.v[0].u8, dest.v[1].u8,
                    left.v[0].u8, left.v[1].u8,
                    right.v[0].u8, right.v[1].u8,
                };
            } else {
                return { static_cast<uint8_t>(vm::Instruction::ERR) };
            }
        }
        case Expression::LOAD_CONST: {
			std::vector<uint8_t> result;
			int size = variable_symbol->type->get_size();
			if (size == 0) return {};

			vm::Value* v = value->eval_constexpr();
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
        case Expression::CALL: {
            return { static_cast<uint8_t>(vm::Instruction::ERR) };
            return {
                static_cast<uint8_t>(vm::Instruction::CALL_1),
                
            };
        }
        default: return { static_cast<uint8_t>(vm::Instruction::ERR) };
    }
}

size_t DeclareStatement::get_bytecode_size(BytecodeGenerationInfo &bgi) const {
    ExprPtr value = variable_symbol->initial_value;
	if (!value) return 0;
    
    switch(value->get_kind()) {
        case Expression::BINARY:
            if (value->get_type() == PrimitiveType::TYPE_INT) return 1 + 2 + 2 + 2;
            else return 1;
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