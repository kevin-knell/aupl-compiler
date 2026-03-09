#include "declare_statement.hpp"
#include "instructions.hpp"
#include "primitive_type.hpp"
#include "value.hpp"
#include "binary_op_expression.hpp"
#include "variable_expression.hpp"
#include "load_const_expression.hpp"
#include "color.hpp"
#include "call_expression.hpp"
#include "class_db.hpp"
#include "string_literal_expression.hpp"
#include "symbol_table.hpp"
#include "bytecode_generator.hpp"

#define TAG(s, b) std::string(b ? s : "")

namespace cmp {

DeclareStatement::DeclareStatement(const VarPtr& variable_symbol)
    : variable_symbol(variable_symbol) {}

std::string DeclareStatement::to_string() const {
    return TAG(C_KEYWORD("volatile "), is_volatile) + variable_symbol->to_string() + " " +
           (variable_symbol->initial_value
               ? "= " + variable_symbol->initial_value->to_string()
               : "");
}

namespace {
    std::vector<uint8_t> generate_binary_bytecode(const BinaryExpression& binary_expr, const VarPtr& variable_symbol, BytecodeGenerationInfo& bgi) {
        auto left_op = std::dynamic_pointer_cast<VariableExpression>(binary_expr.left);
        auto right_op = std::dynamic_pointer_cast<VariableExpression>(binary_expr.right);
        vm::Value2 dest{ .u16 = static_cast<uint16_t>(Scope::get_variable_index(bgi.scope, variable_symbol->name)) };
        vm::Value2 left{ .u16 = static_cast<uint16_t>(Scope::get_variable_index(bgi.scope, left_op->var->name)) };
        vm::Value2 right{ .u16 = static_cast<uint16_t>(Scope::get_variable_index(bgi.scope, right_op->var->name)) };
		uint8_t op_code = 0;

		//bool has_const = left_op->var->initial_value->is_constexpr() || right_op->var->initial_value->is_constexpr();

		vm::BinType bin_type = std::dynamic_pointer_cast<PrimitiveType>(variable_symbol->type)->vm_bin_type;

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
        vm::Value2 dest{ .u16 = static_cast<uint16_t>(Scope::get_variable_index(bgi.scope, variable_symbol->name)) };
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

	std::vector<uint8_t> generate_native_call_bytecode(const CallExpression& value, const VarPtr& variable_symbol, BytecodeGenerationInfo& bgi) {
        std::vector<uint8_t> result;

		result.push_back((uint8_t)vm::Instruction::CALL_NATIVE);
		std::shared_ptr<VariableExpression> arg0 = nullptr;

		if (!value.arguments.empty()) {
			arg0 = std::dynamic_pointer_cast<VariableExpression>(value.arguments[0]);
		}

		auto method_pair = *value.f->method_pair;

		vm::Value2 class_id = vm::Value2::from<uint16_t>(method_pair.class_id);
		vm::Value2 method_id = vm::Value2::from<uint16_t>(method_pair.method_id);

		VarExprPtr obj_var_expr;
		
		if (value.obj_expr
				&& value.obj_expr->get_type()->get_kind() != Type::STATIC_CLASS) {
			obj_var_expr = std::dynamic_pointer_cast<VariableExpression>(value.obj_expr);
		}
		
		vm::Value2 obj_address = vm::Value2::from<uint16_t>(obj_var_expr ? Scope::get_variable_index(bgi.scope, obj_var_expr->name) : 0);
		vm::Value2 args_address = vm::Value2::from<uint16_t>(arg0 ? Scope::get_variable_index(bgi.scope, arg0->var->name) : 0);
		vm::Value2 ret_address = vm::Value2::from<uint16_t>(Scope::get_variable_index(bgi.scope, variable_symbol->name));

		if (method_pair.is_constructor) {
			obj_address = ret_address;
		}

		result.push_back(class_id.v[0].u8);
		result.push_back(class_id.v[1].u8);
		result.push_back(method_id.v[0].u8);
		result.push_back(method_id.v[1].u8);
		result.push_back(obj_address.v[0].u8);
		result.push_back(obj_address.v[1].u8);
		result.push_back(args_address.v[0].u8);
		result.push_back(args_address.v[1].u8);
		result.push_back(ret_address.v[0].u8);
		result.push_back(ret_address.v[1].u8);

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
		case Expression::STRING_LIT: {
			auto string_lit_expr = std::dynamic_pointer_cast<StringLiteralExpression>(value);
			vm::Value2 dest{ .u16 = static_cast<uint16_t>(Scope::get_variable_index(bgi.scope, variable_symbol->name)) };
			vm::Value2 string_pos = vm::Value2::from<uint16_t>(bgi.symbol_table.const_memory.size());
			for (char c : string_lit_expr->value) {
				bgi.symbol_table.const_memory.emplace_back(vm::Value::from(c));
			}
			bgi.symbol_table.const_memory.emplace_back(vm::Value::from<uint8_t>(0));
			return {
				(uint8_t)vm::Instruction::LOAD_STRING,
				dest.v[0].u8, dest.v[1].u8,
				string_pos.v[0].u8, string_pos.v[1].u8,
			};
		}
        case Expression::CALL: {
			auto call_expr = std::dynamic_pointer_cast<CallExpression>(value);
            
			if (call_expr->f->method_pair) {
				return generate_native_call_bytecode(*call_expr, variable_symbol, bgi);
			} else {
				//std::cout << to_string() << " is not native" << std::endl;

				std::shared_ptr<VariableExpression> arg0 = nullptr;

				if (!call_expr->arguments.empty()) {
					arg0 = std::dynamic_pointer_cast<VariableExpression>(call_expr->arguments[0]);
				}

				auto func_address = vm::Value4::from<uint32_t>(call_expr->f->scope->starting_address);
				vm::Value2 args_address = vm::Value2::from<uint16_t>(arg0 ? Scope::get_variable_index(bgi.scope, arg0->var->name) : 0);
				vm::Value2 ret_address = vm::Value2::from<uint16_t>(Scope::get_variable_index(bgi.scope, variable_symbol->name));

				return {
					(uint8_t)vm::Instruction::CALL,
					func_address.v[0].u8, func_address.v[1].u8, func_address.v[2].u8, func_address.v[3].u8,
					args_address.v[0].u8, args_address.v[1].u8,
					ret_address.v[0].u8, ret_address.v[0].u8,
				};
				return { static_cast<uint8_t>(vm::Instruction::ERR) };
			}
		}
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
		case Expression::STRING_LIT: {
			return 1 + 2 + 2;
		}
		case Expression::CALL: {
			auto call_expr = std::dynamic_pointer_cast<CallExpression>(value);
            
			if (call_expr->f->method_pair) {
				return 1 + 2 + 2 + 2 + 2 + 2;
			} else {
				return 1 + 4 + 2 + 2;
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