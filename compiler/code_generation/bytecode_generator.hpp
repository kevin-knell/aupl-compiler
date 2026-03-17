#pragma once
#include <cstddef>
#include <type_traits>
#include <string>
#include "forward_declarations.hpp"
#include "assert.h"
#include "color.hpp"
#include "instructions.hpp"

#include "statement_visitor.hpp"
#include "expression_visitor.hpp"

#include "statement.hpp"
#include "declare_statement.hpp"
#include "assign_statement.hpp"
#include "conditional_jump_statement.hpp"
#include "return_statement.hpp"
#include "label_statement.hpp"
#include "expression_statement.hpp"

#include "expression.hpp"
#include "load_const_expression.hpp"
#include "variable_expression.hpp"
#include "binary_op_expression.hpp"
#include "string_literal_expression.hpp"
#include "unary_op_expression.hpp"
#include "call_expression.hpp"

#include "label.hpp"
#include "class_db.hpp"
#include "symbol_table.hpp"

#include "primitive_type.hpp"
#include <iomanip>
#include <functional>

#ifdef BCG_DEBUG_VERBOSE
#define BCG_DEBUG_PRINT(m_text) std::cout << m_text << std::endl
#define BCG_DEBUG_PRINT_V(m_text) BCG_DEBUG_PRINT(m_text)
#define BCG_DEBUG_PRINT_NV(m_text)
#elifdef BCG_DEBUG
#define BCG_DEBUG_PRINT(m_text) std::cout << m_text << std::endl
#define BCG_DEBUG_PRINT_V(m_text)
#define BCG_DEBUG_PRINT_NV(m_text) BCG_DEBUG_PRINT(m_text)
#else
#define BCG_DEBUG_PRINT(m_text)
#define BCG_DEBUG_PRINT_V(m_text)
#define BCG_DEBUG_PRINT_NV(m_text)
#endif

namespace cmp
{

using VarExprPtr = std::shared_ptr<VariableExpression>;

struct BytecodeGenerationInfo {
	SymbolTable& symbol_table;
	ClassPtr cls;
	FuncPtr f;
	ScopePtr scope;
	size_t bytecode_size = 0;

	BytecodeGenerationInfo(SymbolTable& symbol_table) : symbol_table(symbol_table) {}
	BytecodeGenerationInfo(SymbolTable& symbol_table, ClassPtr cls, FuncPtr f, ScopePtr scope)
			: symbol_table(symbol_table), cls(cls), f(f), scope(scope) {}
};

struct BytecodeProductInfo {
	std::vector<uint8_t> bytecode;
    size_t main_start;
    bool has_main = false;
};

template<bool size_only>
class BytecodeGenerator : StatementVisitor, ExpressionVisitor {
private:
	using BC_GEN_RES = std::conditional_t<size_only, size_t, BytecodeProductInfo>;
	
	BytecodeGenerationInfo bgi;
	BC_GEN_RES result;
public:
	BytecodeGenerator(SymbolTable& symbol_table) : bgi(BytecodeGenerationInfo(symbol_table)) {}
	BC_GEN_RES generate_bytecode(SymbolTable& symbol_table, size_t bytecode_size);

private:
	void add_error();
	void append(vm::Instruction op_code, std::vector<uint8_t>&& bytecode);

	// statements
	void generate_bytecode(const Statement& stmt);
	void generate_bytecode(const DeclareStatement& stmt);
	void generate_bytecode(const AssignmentStatement& stmt);
	void generate_bytecode(const ConditionalJumpStatement& stmt);
	void generate_bytecode(const ReturnStatement& stmt);
	void generate_bytecode(const LabelStatement& stmt);
	void generate_bytecode(const ExpressionStatement& stmt);
	
	// expressions
	// dest <- expr
	void generate_bytecode(const Expression& expr, VarExprPtr dest_var);
	void generate_bytecode(const VariableExpression& expr, VarExprPtr dest_var);
	void generate_bytecode(const UnaryExpression& expr, VarExprPtr dest_var);
	void generate_bytecode(const BinaryExpression& expr, VarExprPtr dest_var);
	void generate_bytecode(const LoadConstExpression& expr, VarExprPtr dest_var);
	void generate_bytecode(const CallExpression& expr, VarExprPtr dest_var);
	void generate_bytecode(const AccessExpression& expr, VarExprPtr dest_var);
	void generate_bytecode(const TupleExpression& expr, VarExprPtr dest_var);
	void generate_bytecode(const StringLiteralExpression& expr, VarExprPtr dest_var);
	
	void visit(Statement& stmt) override { generate_bytecode(stmt); };
	void visit(DeclareStatement& stmt) override { generate_bytecode(stmt); };
	void visit(AssignmentStatement& stmt) override { generate_bytecode(stmt); };
	void visit(ConditionalJumpStatement& stmt) override { generate_bytecode(stmt); };
	void visit(ReturnStatement& stmt) override { generate_bytecode(stmt); };
	void visit(LabelStatement& stmt) override { generate_bytecode(stmt); };
	void visit(ExpressionStatement& stmt) override { generate_bytecode(stmt); };
	
	void visit(VariableExpression& expr) override { std::cerr << "Expression without var visited!" << std::endl; exit(1); };
	void visit(UnaryExpression& expr) override { std::cerr << "Expression without var visited!" << std::endl; exit(1); };
	void visit(BinaryExpression& expr) override { std::cerr << "Expression without var visited!" << std::endl; exit(1); };
	void visit(LoadConstExpression& expr) override { std::cerr << "Expression without var visited!" << std::endl; exit(1); };
	void visit(CallExpression& expr) override { std::cerr << "Expression without var visited!" << std::endl; exit(1); };
	void visit(AccessExpression& expr) override { std::cerr << "Expression without var visited!" << std::endl; exit(1); };
	void visit(TupleExpression& expr) override { std::cerr << "Expression without var visited!" << std::endl; exit(1); };
	void visit(StringLiteralExpression& expr) override { std::cerr << "Expression without var visited!" << std::endl; exit(1); };

	void visit(VariableExpression& expr, VarExprPtr var_expr) override { generate_bytecode(expr, var_expr); }
	void visit(UnaryExpression& expr, VarExprPtr var_expr) override { generate_bytecode(expr, var_expr); }
	void visit(BinaryExpression& expr, VarExprPtr var_expr) override { generate_bytecode(expr, var_expr); }
	void visit(LoadConstExpression& expr, VarExprPtr var_expr) override { generate_bytecode(expr, var_expr); }
	void visit(CallExpression& expr, VarExprPtr var_expr) override { generate_bytecode(expr, var_expr); }
	void visit(AccessExpression& expr, VarExprPtr var_expr) override { generate_bytecode(expr, var_expr); }
	void visit(TupleExpression& expr, VarExprPtr var_expr) override { generate_bytecode(expr, var_expr); }
	void visit(StringLiteralExpression& expr, VarExprPtr var_expr) override { generate_bytecode(expr, var_expr); }
};

template <bool size_only>
inline BytecodeGenerator<size_only>::BC_GEN_RES BytecodeGenerator<size_only>::generate_bytecode(SymbolTable &symbol_table, size_t bytecode_size) {
	if constexpr(size_only) {
		result = size_t(0);
	} else {
		result = BytecodeProductInfo {
			.bytecode = std::vector<uint8_t>(),
			.main_start = 0,
			.has_main = false
		};
	}

	// classes
    for (auto [cn, cls] : symbol_table.classes) {
		if (cls->native_class_bind) continue;
        BCG_DEBUG_PRINT(cn);
		bgi.cls = cls;

		// functions
        for (auto [fn, f] : cls->functions) {
			if constexpr(!size_only) {
				if (fn == "main") {
					result.main_start = result.bytecode.size();
					result.has_main = true;
				}
			}

            BCG_DEBUG_PRINT("\n" << f->to_string());
			bgi.f = f;

			// scopes
			std::function<void(cmp::ScopePtr)> iterate_scopes = [&](cmp::ScopePtr scope) {
				bgi.scope = scope;

				if constexpr(size_only) {
					scope->starting_address = result;
					BCG_DEBUG_PRINT(scope->get_full_name() << ": " << std::hex << (int)(result) << std::dec);
				} else {
					BCG_DEBUG_PRINT(scope->get_full_name() << " " << (int)scope->starting_address << " {");
				}

				for (auto stmt : scope->body) {
					if constexpr(size_only) {
						bgi.bytecode_size = result;
						BCG_DEBUG_PRINT("\t" << stmt->to_string() << ": " << std::hex << result << std::dec);
					} else {
						BCG_DEBUG_PRINT_V("\t" << stmt->to_string() << ": ");
					}

					stmt->accept(*this);
				}

				if (scope->type == Scope::FUNCTION) {
					if(fn == "main") {
						append(vm::Instruction::HALT, {});
					} else {
						append(vm::Instruction::RET, {});
					}
				}

				if constexpr(size_only) {
					BCG_DEBUG_PRINT("");
				} else {
					BCG_DEBUG_PRINT("}");
				}
                
				for (auto& lower : scope->lower_scopes) {
					if (auto lower_scope = lower.lock()) {
						iterate_scopes(lower_scope);
					} else {
						BCG_DEBUG_PRINT("Warning: lower scope is expired");
					}
                }
            };

			iterate_scopes(f->scope);
        }
    }

	return result;
}

template <bool size_only>
inline void BytecodeGenerator<size_only>::add_error() {
	append(vm::Instruction::ERR, {});
}

template <bool size_only>
inline void BytecodeGenerator<size_only>::append(vm::Instruction op_code, std::vector<uint8_t>&& bytecode) {
	std::vector<int> op_sizes = vm::OP_SIZES[static_cast<uint8_t>(op_code)];
	
	if constexpr(size_only) {
		size_t sum = 0;
		for (int s : op_sizes) {
			sum += s;
		}
		result += sum;
		assert(bytecode.size() + 1 == sum);
	} else {
		std::cout << std::hex << std::uppercase << std::setw(2) << std::setfill('0') <<
				static_cast<int>(op_code) << " " <<
				C_KEYWORD(vm::OP_NAMES[static_cast<uint8_t>(op_code)]) << std::flush << " ";

		size_t offset = 0;

		for (size_t arg_idx = 1; arg_idx < op_sizes.size(); ++arg_idx) {
			size_t size = op_sizes[arg_idx];
			for (size_t byte_idx = 0; byte_idx < size; ++byte_idx) {
				std::cout <<
						std::hex << std::uppercase << std::setw(2) << std::setfill('0') <<
						(int)bytecode[offset + byte_idx];
			}
			std::cout << " ";
			offset += size;
		}
		std::cout << std::endl;

		result.bytecode.push_back(static_cast<uint8_t>(op_code));
		result.bytecode.insert(result.bytecode.end(), bytecode.begin(), bytecode.end());
	}
}

// ================================================================================================
// Statements
// ================================================================================================
template <bool size_only>
inline void BytecodeGenerator<size_only>::generate_bytecode(const Statement &stmt) {
	add_error();
}

template <bool size_only>
inline void BytecodeGenerator<size_only>::generate_bytecode(const DeclareStatement& stmt) {
	if (!stmt.variable_symbol->initial_value) return;

	VarExprPtr var_expr = std::make_shared<VariableExpression>(stmt.variable_symbol);
	stmt.variable_symbol->initial_value->accept(*this, var_expr);
}

template <bool size_only>
inline void BytecodeGenerator<size_only>::generate_bytecode(const AssignmentStatement &stmt) {
	stmt.expr_right->accept(*this, std::dynamic_pointer_cast<VariableExpression>(stmt.expr_left));
}

template <bool size_only>
inline void BytecodeGenerator<size_only>::generate_bytecode(const ConditionalJumpStatement &stmt) {
	if (!stmt.condition) {
		vm::Value4 addr{ .u32 = stmt.if_label->get_address() };

		append(
			vm::Instruction::GOTO,
			{
				addr.v[0].u8,
				addr.v[1].u8,
				addr.v[2].u8,
				addr.v[3].u8,
			}
		);

		return;
	}

	struct ConditionVisitor : ExpressionVisitor {
		BytecodeGenerator& self;
		const ConditionalJumpStatement& stmt;

		ConditionVisitor(BytecodeGenerator& self, const ConditionalJumpStatement& stmt)
				: self(self), stmt(stmt) {}

		void visit(Expression& expr) override { self.add_error(); }
		void visit(VariableExpression& expr) override {
			vm::Value2 src{ .u16 = static_cast<uint16_t>(Scope::get_variable_index(self.bgi.scope, expr.name)) };
			vm::Value4 if_addr{ .u32 = stmt.if_label->get_address() };

			self.append(
				vm::Instruction::IF_NEQ_CONST_I8,
				{
					src.v[0].u8,
					src.v[1].u8,
					0,
					if_addr.v[0].u8,
					if_addr.v[1].u8,
					if_addr.v[2].u8,
					if_addr.v[3].u8,
				}
			);
			
			if (stmt.else_label) {
				vm::Value4 else_addr{ .u32 = stmt.else_label->get_address() };
				self.append(
					vm::Instruction::GOTO,
					{
						else_addr.v[0].u8,
						else_addr.v[1].u8,
						else_addr.v[2].u8,
						else_addr.v[3].u8,
					}
				);
			}
		}
		void visit(UnaryExpression& expr) override { self.add_error(); }
		void visit(BinaryExpression& expr) override {
			auto left_var = std::dynamic_pointer_cast<VariableExpression>(expr.left)->var;
			auto right_var = std::dynamic_pointer_cast<VariableExpression>(expr.right)->var;
			vm::Value2 left{ .u16 = static_cast<uint16_t>(Scope::get_variable_index(self.bgi.scope, left_var->name)) };
			vm::Value2 right{ .u16 = static_cast<uint16_t>(Scope::get_variable_index(self.bgi.scope, right_var->name)) };
			vm::Value4 if_addr{ .u32 = stmt.if_label->get_address() };

			std::vector<uint8_t> result;

			auto bin_type = std::dynamic_pointer_cast<PrimitiveType>(left_var->type)->vm_bin_type;

			auto instruction = vm::get_binary_opcode(bin_type, expr.op, false);

			BCG_DEBUG_PRINT(left_var->name << " ? " << right_var->name);

			self.append(
				instruction,
				{
					left.v[0].u8,
					left.v[1].u8,
					right.v[0].u8,
					right.v[1].u8,
					if_addr.v[0].u8,
					if_addr.v[1].u8,
					if_addr.v[2].u8,
					if_addr.v[3].u8,
				}
			);

			if (stmt.else_label) {
				vm::Value4 else_addr{ .u32 = stmt.else_label->get_address() };
				self.append(
					vm::Instruction::GOTO,
					{
						else_addr.v[0].u8,
						else_addr.v[1].u8,
						else_addr.v[2].u8,
						else_addr.v[3].u8,
					}
				);
			}
		}
		void visit(LoadConstExpression& expr) override { self.add_error(); }
		void visit(CallExpression& expr) override { self.add_error(); }
		void visit(AccessExpression& expr) override { self.add_error(); }
		void visit(TupleExpression& expr) override { self.add_error(); }
		void visit(StringLiteralExpression& expr) override { self.add_error(); }
	};

	ConditionVisitor condition_visitor(*this, stmt);
	
	stmt.condition->accept(condition_visitor);
}

template <bool size_only>
inline void BytecodeGenerator<size_only>::generate_bytecode(const ReturnStatement &stmt) {
	append(vm::Instruction::RET, {});
}

template <bool size_only>
inline void BytecodeGenerator<size_only>::generate_bytecode(const LabelStatement &stmt) {
	if constexpr(size_only) {
		if (bgi.scope->label_addresses.find(stmt.identifier) != bgi.scope->label_addresses.end()) {
			std::cerr << "ERROR, already has label " << stmt.to_string() << ": " << bgi.scope->label_addresses[stmt.identifier] << std::endl;
			exit(1);
		}
		bgi.scope->label_addresses[stmt.identifier] = bgi.bytecode_size;
	}
}

template <bool size_only>
inline void BytecodeGenerator<size_only>::generate_bytecode(const ExpressionStatement &stmt) {
	auto var_expr = std::make_shared<VariableExpression>(bgi.scope->variables.begin()->second);
	stmt.expression->accept(*this, var_expr);
}

// ================================================================================================
// dest <- expr
// ================================================================================================
template <bool size_only>
inline void BytecodeGenerator<size_only>::generate_bytecode(
		const Expression &expr,
		VarExprPtr dest_var) {
	add_error();
}

template <bool size_only>
inline void BytecodeGenerator<size_only>::generate_bytecode(const VariableExpression &expr, VarExprPtr dest_var) {
	auto right_type = expr.get_type();
	
	if (!right_type->get_kind() == Type::KIND::PRIMITIVE) {
		add_error();
	}

	auto right_primitive_type = std::dynamic_pointer_cast<PrimitiveType>(right_type);

	vm::Value2 dest{ .u16 = Scope::get_variable_index(bgi.scope, dest_var->name) };
	vm::Value2 left{ .u16 = static_cast<uint16_t>(Scope::get_variable_index(bgi.scope, expr.var->name)) };
	vm::Value4 right{ .u32 = static_cast<uint32_t>(0) };
	vm::Instruction op_code = vm::get_binary_opcode(
		right_primitive_type->vm_bin_type,
		BinaryExpression::OPERATOR::ADD,
		true //has_const
	);

	append(
		op_code,
		{
			dest.v[0].u8, dest.v[1].u8,
			left.v[0].u8, left.v[1].u8,
			right.v[0].u8, right.v[1].u8, right.v[2].u8, right.v[3].u8,
		}
	);
}

template <bool size_only>
inline void BytecodeGenerator<size_only>::generate_bytecode(
		const UnaryExpression& expr,
		VarExprPtr dest_var) {
	add_error();
}

template <bool size_only>
inline void BytecodeGenerator<size_only>::generate_bytecode(
		const BinaryExpression &binary_expr,
		VarExprPtr dest_var) {
	// check expressions
	assert(binary_expr.left->get_kind() == Expression::VARIABLE);
	assert(binary_expr.right->get_kind() == Expression::VARIABLE);
	
	// check types
	assert(dest_var->get_type()->get_kind() == Type::PRIMITIVE);
	assert(binary_expr.left->get_type()->get_kind() == Type::PRIMITIVE);
	assert(binary_expr.right->get_type()->get_kind() == Type::PRIMITIVE);

	auto left_op = std::dynamic_pointer_cast<VariableExpression>(binary_expr.left);
	auto right_op = std::dynamic_pointer_cast<VariableExpression>(binary_expr.right);
	
	vm::Value2 dest{ .u16 = static_cast<uint16_t>(Scope::get_variable_index(bgi.scope, dest_var->name)) };
	vm::Value2 left{ .u16 = static_cast<uint16_t>(Scope::get_variable_index(bgi.scope, left_op->var->name)) };
	vm::Value2 right{ .u16 = static_cast<uint16_t>(Scope::get_variable_index(bgi.scope, right_op->var->name)) };
	vm::Instruction op_code;

	//bool has_const = left_op->var->initial_value->is_constexpr() || right_op->var->initial_value->is_constexpr();

	vm::BinType bin_type = std::dynamic_pointer_cast<PrimitiveType>(dest_var->get_type())->vm_bin_type;

	op_code = vm::get_binary_opcode(
		bin_type,
		binary_expr.op,
		false //has_const
	);

	append(
		op_code,
		{
			dest.v[0].u8, dest.v[1].u8,
			left.v[0].u8, left.v[1].u8,
			right.v[0].u8, right.v[1].u8,
		}
	);
}

template <bool size_only>
inline void BytecodeGenerator<size_only>::generate_bytecode(
		const LoadConstExpression &expr,
		VarExprPtr dest_var) {
	int size = dest_var->get_type()->get_size();
	assert(expr.get_type()->get_size() >= size);
	if (size == 0) return;
	
	vm::Instruction op_code;
	switch (size) {
		case 1: op_code = vm::Instruction::LOAD_CONST_1; break;
		case 2: op_code = vm::Instruction::LOAD_CONST_2; break;
		case 4: op_code = vm::Instruction::LOAD_CONST_4; break;
		case 8: op_code = vm::Instruction::LOAD_CONST_8; break;
		case 16: op_code = vm::Instruction::LOAD_CONST_16; break;
		default:
			add_error();
			return;
	}

	std::vector<uint8_t> args;
	
	vm::Value2 dest{ .u16 = static_cast<uint16_t>(Scope::get_variable_index(bgi.scope, dest_var->name)) };
	args = { dest.v[0].u8, dest.v[1].u8 };
	
	vm::Value* v = expr.eval_constexpr();
	for (int i = 0; i < size; ++i) {
		args.push_back(v[i].u8);
	}
	
	append(op_code, std::move(args));
}

template <bool size_only>
inline void BytecodeGenerator<size_only>::generate_bytecode(
		const CallExpression &expr,
		VarExprPtr dest_var) {
	if(!expr.f) {
		add_error();
		return;
	}

	std::shared_ptr<VariableExpression> arg0;
	
	if (!expr.arguments.empty()) {
		arg0 = std::dynamic_pointer_cast<VariableExpression>(expr.arguments[0]);
	}
	
	if (expr.f->method_pair) {
		const vm::MethodPair& method_pair = *expr.f->method_pair;

		vm::Value2 class_id = vm::Value2::from<uint16_t>(method_pair.class_id);
		vm::Value2 method_id = vm::Value2::from<uint16_t>(method_pair.method_id);

		VarExprPtr obj_var_expr;
		
		if (expr.obj_expr
				&& expr.obj_expr->get_type()->get_kind() != Type::STATIC_CLASS) {
			obj_var_expr = std::dynamic_pointer_cast<VariableExpression>(expr.obj_expr);
		}
		
		vm::Value2 obj_address = vm::Value2::from<uint16_t>(obj_var_expr ? Scope::get_variable_index(bgi.scope, obj_var_expr->name) : 0);
		vm::Value2 args_address = vm::Value2::from<uint16_t>(arg0 ? Scope::get_variable_index(bgi.scope, arg0->var->name) : 0);
		vm::Value2 ret_address = vm::Value2::from<uint16_t>(Scope::get_variable_index(bgi.scope, dest_var->name));

		if (method_pair.is_constructor) {
			obj_address = ret_address;
		}

		append(
			vm::Instruction::CALL_NATIVE,
			{
				class_id.v[0].u8, class_id.v[1].u8,
				method_id.v[0].u8, method_id.v[1].u8,
				obj_address.v[0].u8, obj_address.v[1].u8,
				args_address.v[0].u8, args_address.v[1].u8,
				ret_address.v[0].u8, ret_address.v[1].u8
			}
		);
	} else {
		auto func_address = vm::Value4::from<uint32_t>(expr.f->scope->starting_address);
		vm::Value2 args_address = vm::Value2::from<uint16_t>(arg0 ? Scope::get_variable_index(bgi.scope, arg0->var->name) : 0);
		vm::Value2 ret_address = vm::Value2::from<uint16_t>(Scope::get_variable_index(bgi.scope, dest_var->name));

		append(
			vm::Instruction::CALL,
			{
				func_address.v[0].u8, func_address.v[1].u8, func_address.v[2].u8, func_address.v[3].u8,
				args_address.v[0].u8, args_address.v[1].u8,
				ret_address.v[0].u8, ret_address.v[0].u8
			}
		);
	}
}

template <bool size_only>
inline void BytecodeGenerator<size_only>::generate_bytecode(const AccessExpression &expr, VarExprPtr dest_var) {
	add_error();
}

template <bool size_only>
inline void BytecodeGenerator<size_only>::generate_bytecode(const TupleExpression &expr, VarExprPtr dest_var) {
	add_error();
}

template <bool size_only>
inline void BytecodeGenerator<size_only>::generate_bytecode(const StringLiteralExpression &expr, VarExprPtr dest_var) {
	vm::Value2 dest{ .u16 = static_cast<uint16_t>(Scope::get_variable_index(bgi.scope, dest_var->name)) };
	vm::Value2 string_pos = vm::Value2::from<uint16_t>(bgi.symbol_table.const_memory.size());
	if constexpr(!size_only) {
		for (char c : expr.value) {
			bgi.symbol_table.const_memory.emplace_back(vm::Value::from(c));
		}
		bgi.symbol_table.const_memory.emplace_back(vm::Value::from<uint8_t>(0));
	}
	append(
		vm::Instruction::LOAD_STRING,
		{
			dest.v[0].u8, dest.v[1].u8,
			string_pos.v[0].u8, string_pos.v[1].u8
		}
	);
}

} // namespace cmp
