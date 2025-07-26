#include "assign_statement.hpp"
#include "instructions.hpp"
#include "primitive_type.hpp"
#include "variable_expression.hpp"
#include "binary_op_expression.hpp"
#include <assert.h>

namespace cmp {

AssignmentStatement::AssignmentStatement(ExprPtr expr_left, ExprPtr expr_right)
	: expr_left(std::move(expr_left)), expr_right(std::move(expr_right)) {}

std::string AssignmentStatement::to_string() const {
	return expr_left->to_string() + " = " + expr_right->to_string();
}

namespace {
    std::vector<uint8_t> generate_binary_bytecode(
			const BinaryExpression& binary_expr,
			const uint16_t offset,
			const TypePtr type,
			BytecodeGenerationInfo& bgi) {
        auto left_op = std::dynamic_pointer_cast<VariableExpression>(binary_expr.left);
        auto right_op = std::dynamic_pointer_cast<VariableExpression>(binary_expr.right);
        vm::Value2 dest{ .u16 = offset };
        vm::Value2 left{ .u16 = static_cast<uint16_t>(bgi.scope->variable_indices[left_op->var->name]) };
        vm::Value2 right{ .u16 = static_cast<uint16_t>(bgi.scope->variable_indices[right_op->var->name]) };
		uint8_t op_code = 0;

		//bool has_const = left_op->var->initial_value->is_constexpr() || right_op->var->initial_value->is_constexpr();

		vm::BinType bin_type;

		if (type == PrimitiveType::TYPE_U8) {
			bin_type = vm::BinType::UINT8;
		} else if (type == PrimitiveType::TYPE_I8) {
			bin_type = vm::BinType::INT8;
		} else if (type == PrimitiveType::TYPE_U16) {
			bin_type = vm::BinType::UINT16;
		} else if (type == PrimitiveType::TYPE_I16) {
			bin_type = vm::BinType::INT16;
		} else if (type == PrimitiveType::TYPE_U32) {
			bin_type = vm::BinType::UINT32;
		} else if (type == PrimitiveType::TYPE_I32) {
			bin_type = vm::BinType::INT32;
		} else if (type == PrimitiveType::TYPE_U64) {
			bin_type = vm::BinType::UINT64;
		} else if (type == PrimitiveType::TYPE_INT) {
			bin_type = vm::BinType::INT64;
		} else if (type == PrimitiveType::TYPE_F32) {
			bin_type = vm::BinType::FLOAT;
		} else if (type == PrimitiveType::TYPE_FLOAT) {
			bin_type = vm::BinType::DOUBLE;
		} else {
			std::cerr << "Unsupported type for binary operation: " << type->to_string() << std::endl;
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

    std::vector<uint8_t> generate_load_const_bytecode(
			const ExprPtr& value,
			const uint16_t offset,
			const int size,
			BytecodeGenerationInfo& bgi) {
        std::vector<uint8_t> result;
        if (size == 0) return {};
		
        vm::Value* v = value->eval_constexpr();
        vm::Value2 dest{ .u16 = offset };
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

    std::vector<uint8_t> generate_copy_to_static_bytecode(
			const uint16_t offset_static,
			const uint16_t offset_local,
			const int size,
			BytecodeGenerationInfo& bgi) {
        std::vector<uint8_t> result;
        if (size == 0) return {};
		
        vm::Value2 dest{ .u16 = offset_static };
        vm::Value2 local{ .u16 = offset_local };
        switch (size) {
            case 1: result.push_back((uint8_t)vm::Instruction::COPY_TO_STATIC_1); break;
            case 2: result.push_back((uint8_t)vm::Instruction::COPY_TO_STATIC_2); break;
            case 4: result.push_back((uint8_t)vm::Instruction::COPY_TO_STATIC_4); break;
            case 8: result.push_back((uint8_t)vm::Instruction::COPY_TO_STATIC_8); break;
            case 16: result.push_back((uint8_t)vm::Instruction::COPY_TO_STATIC_16); break;
            default:
                result.push_back((uint8_t)vm::Instruction::COPY_TO_STATIC_VAR);
                result.push_back(size);
        }
        result.push_back(dest.v[0].u8);
        result.push_back(dest.v[1].u8);
        result.push_back(local.v[0].u8);
        result.push_back(local.v[1].u8);
        return result;
    }
}

std::vector<uint8_t> AssignmentStatement::generate_bytecode(BytecodeGenerationInfo& bgi) const {
	switch (expr_left->get_kind()) {
		case Expression::VARIABLE: {
			auto var_expr = std::dynamic_pointer_cast<VariableExpression>(expr_left);
			assert(var_expr);
			VarPtr var = var_expr->var;
			assert(var);
			assert(var->scope);
			switch (var->scope->type) {
				case Scope::FUNCTION: // fall through
				case Scope::FUNCTION_SUB: {
					switch (expr_right->get_kind()) {
						case Expression::BINARY: {
							auto binary_expr = std::dynamic_pointer_cast<BinaryExpression>(expr_right);
							return generate_binary_bytecode(
								*binary_expr,
								var->scope->variable_indices[var->name],
								var->type,
								bgi
							);
						}
						case Expression::LOAD_CONST: {
							return generate_load_const_bytecode(
								expr_right,
								var->scope->variable_indices[var->name],
								var->type->get_size(),
								bgi
							);
						}
						default:
							return { static_cast<uint8_t>(vm::Instruction::ERR) };
					}
				}
				case Scope::CLASS: {
					return { static_cast<uint8_t>(vm::Instruction::ERR) };
				}
				case Scope::STATIC_CLASS: {
					switch (expr_right->get_kind()) {
						case Expression::VARIABLE: {
							auto local_var_expr = std::dynamic_pointer_cast<VariableExpression>(expr_right);
							assert(local_var_expr);
							auto local_var = local_var_expr->var;
							assert(local_var);
							assert(local_var->scope);
							return generate_copy_to_static_bytecode(
								var->scope->variable_indices[var->name],
								local_var->scope->variable_indices[local_var->name],
								var->type->get_size(),
								bgi
							);
						}
						default:
							return { static_cast<uint8_t>(vm::Instruction::ERR) };
					}
				}
				default:
					std::cout << "error: " << var->scope->get_full_name() << " " << var->to_string() << std::endl;
					return { static_cast<uint8_t>(vm::Instruction::ERR) };
			}
		}
		default:
			return { static_cast<uint8_t>(vm::Instruction::ERR) };
	}
}

size_t AssignmentStatement::get_bytecode_size(BytecodeGenerationInfo &bgi) const {
	switch (expr_left->get_kind()) {
		case Expression::VARIABLE: {
			auto var_expr = std::dynamic_pointer_cast<VariableExpression>(expr_left);
			assert(var_expr);
			VarPtr var = var_expr->var;
			assert(var);
			assert(var->scope);
			switch (var->scope->type) {
				case Scope::FUNCTION: // fall through
				case Scope::FUNCTION_SUB: {
					switch (expr_right->get_kind()) {
						case Expression::BINARY: {
							auto binary_expr = std::dynamic_pointer_cast<BinaryExpression>(expr_right);
							return 1 + 2 + 2 + 2;
						}
						case Expression::LOAD_CONST: {
							switch (var->type->get_size()) {
								case 1:
								case 2:
								case 4:
								case 8:
								case 16:
									return 1 + 2 + var->type->get_size();
								default:
									return 1 + 2 + 2 + 2;
							}
						}
						default:
							return 1;
					}
				}
				case Scope::CLASS:
					return 1;
				case Scope::STATIC_CLASS:
					return 1 + 2 + 2;
				default: return 1;
			}
		}
		default:
			return 1;
	}
}

std::vector<ExprPtr*> AssignmentStatement::get_expressions() {
	return {&expr_right};
}

} // namespace cmp
