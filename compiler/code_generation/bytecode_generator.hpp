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
#include "shared_type.hpp"
#include "class_db.hpp"
#include <iomanip>
#include <functional>

#define BCG_DEBUG_PRINT(m_text) \
	if (BytecodeGenerator::bcg_debug_print) { \
		std::cout << m_text << std::endl; \
	}

#define BCG_DEBUG_PRINT_V(m_text) \
	if (BytecodeGenerator::bcg_debug_print_verbose) { \
		BCG_DEBUG_PRINT(m_text); \
	}

#define BCG_DEBUG_PRINT_NV(m_text) \
	if (!BytecodeGenerator::bcg_debug_print_verbose) { \
		BCG_DEBUG_PRINT(m_text); \
	}

namespace cmp
{

using VarExprPtr = std::shared_ptr<VariableExpression>;

struct BytecodeProductInfo {
    size_t main_start;
    bool has_main = false;
	std::vector<uint8_t> bytecode;
	std::vector<vm::Value> const_memory;
};

template<bool size_only>
class BytecodeGenerator final : public StatementVisitor, public ExpressionAssignmentVisitor {
private:
	using BC_GEN_RES = std::conditional_t<size_only, size_t, BytecodeProductInfo>;
	
	const SymbolTable& symbol_table;
	ClassPtr current_class;
	FuncPtr current_function;
	ScopePtr scope;

	bool dp_set = false;
	bool ap_set = false;
	bool bp_set = false;
	
	BC_GEN_RES result;
public:
	static bool bcg_debug_print;
	static bool bcg_debug_print_verbose;

	BytecodeGenerator(const SymbolTable& symbol_table) : symbol_table(symbol_table) {}
	BC_GEN_RES generate_bytecode();

	~BytecodeGenerator() = default;

private:
	void add_error();
	void append(vm::Instruction op_code, std::vector<uint8_t>&& bytecode);
	void adjust_pointers(const VariableExpression* dv, const VariableExpression* av, const VariableExpression* bv);
	void reset_pointers();

	// statements
	void visit(Statement& stmt) override { generate_bytecode(stmt); };
	void visit(DeclareStatement& stmt) override { generate_bytecode(stmt); };
	void visit(AssignmentStatement& stmt) override { generate_bytecode(stmt); };
	void visit(ConditionalJumpStatement& stmt) override { generate_bytecode(stmt); };
	void visit(ReturnStatement& stmt) override { generate_bytecode(stmt); };
	void visit(LabelStatement& stmt) override { generate_bytecode(stmt); };
	void visit(ExpressionStatement& stmt) override { generate_bytecode(stmt); };

	void generate_bytecode(const Statement& stmt);
	void generate_bytecode(const DeclareStatement& stmt);
	void generate_bytecode(const AssignmentStatement& stmt);
	void generate_bytecode(const ConditionalJumpStatement& stmt);
	void generate_bytecode(const ReturnStatement& stmt);
	void generate_bytecode(const LabelStatement& stmt);
	void generate_bytecode(const ExpressionStatement& stmt);
	
	// expression assignments: dest <- expr
	void visit(Expression& expr, VarExprPtr var_expr) override { generate_bytecode(expr, var_expr); }
	void visit(VariableExpression& expr, VarExprPtr var_expr) override { generate_bytecode(expr, var_expr); }
	void visit(UnaryExpression& expr, VarExprPtr var_expr) override { generate_bytecode(expr, var_expr); }
	void visit(BinaryExpression& expr, VarExprPtr var_expr) override { generate_bytecode(expr, var_expr); }
	void visit(LoadConstExpression& expr, VarExprPtr var_expr) override { generate_bytecode(expr, var_expr); }
	void visit(CallExpression& expr, VarExprPtr var_expr) override { generate_bytecode(expr, var_expr); }
	void visit(TupleExpression& expr, VarExprPtr var_expr) override { generate_bytecode(expr, var_expr); }
	void visit(StringLiteralExpression& expr, VarExprPtr var_expr) override { generate_bytecode(expr, var_expr); }

	void generate_bytecode(const Expression& expr, VarExprPtr dest_var);
	void generate_bytecode(const VariableExpression& expr, VarExprPtr dest_var);
	void generate_bytecode(const UnaryExpression& expr, VarExprPtr dest_var);
	void generate_bytecode(const BinaryExpression& expr, VarExprPtr dest_var);
	void generate_bytecode(const LoadConstExpression& expr, VarExprPtr dest_var);
	void generate_bytecode(const CallExpression& expr, VarExprPtr dest_var);
	void generate_bytecode(const TupleExpression& expr, VarExprPtr dest_var);
	void generate_bytecode(const StringLiteralExpression& expr, VarExprPtr dest_var);
};

template <bool size_only>
inline BytecodeGenerator<size_only>::BC_GEN_RES BytecodeGenerator<size_only>::generate_bytecode() {
	if constexpr(size_only) {
		result = size_t(0);
	} else {
		result = BytecodeProductInfo {
			.main_start = 0,
			.has_main = false,
			.bytecode = std::vector<uint8_t>(),
			.const_memory = std::vector<vm::Value>()
		};
	}

	// classes
    for (auto [cn, cls] : symbol_table.classes) {
		if (cls->native_class_bind) continue;
        BCG_DEBUG_PRINT(cn);

		current_class = cls;

		// functions
        for (auto [fn, f] : cls->functions) {
			if constexpr(!size_only) {
				if (fn == "main") {
					result.main_start = result.bytecode.size();
					result.has_main = true;
				}
			}

			BCG_DEBUG_PRINT("\n" << f->head_to_string());

			current_function = f;

			// scopes
			std::function<void()> iterate_scopes = [&]() {
				if constexpr(size_only) {
					scope->starting_address = result;
					BCG_DEBUG_PRINT(scope->get_full_name() << ": " << std::hex << static_cast<int>(result) << std::dec);
				} else {
					BCG_DEBUG_PRINT(scope->get_full_name() << " " << static_cast<int>(scope->starting_address) << " " << C_BRACE_L);
				}

				for (auto stmt : scope->body) {
					if constexpr(size_only) {
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
					BCG_DEBUG_PRINT(C_BRACE_R);
				}
                
				for (auto& lower : scope->lower_scopes) {
					if (auto lower_scope = lower.lock()) {
						scope = lower_scope;
						iterate_scopes();
					} else {
						BCG_DEBUG_PRINT("Warning: lower scope is expired");
					}
                }
            };

			scope = f->scope;

			vm::Value2 frame_size = vm::Value2::from(f->scope->size);

			append(
				vm::Instruction::ADD_SP,
				{
					frame_size.v[0].u8, frame_size.v[1].u8,
				}
			);

			iterate_scopes();

			frame_size = vm::Value2::from(-frame_size.i16);

			append(
				vm::Instruction::ADD_SP,
				{
					frame_size.v[0].u8, frame_size.v[1].u8,
				}
			);
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
		if (bytecode.size() + 1 != sum) {
			throw std::runtime_error("wrong size of args for op_code! " + vm::OP_NAMES[static_cast<uint8_t>(op_code)]);
		}
	} else {
#ifdef BCG_DEBUG
		std::cout << std::hex << std::uppercase << std::setw(2) << std::setfill('0') <<
				result.bytecode.size() << ": " <<
				std::setw(2) << std::setfill('0') <<
				static_cast<int>(op_code) << " " <<
				C_KEYWORD(vm::OP_NAMES[static_cast<uint8_t>(op_code)]) << std::flush << " ";

		size_t offset = 0;

		for (size_t arg_idx = 1; arg_idx < op_sizes.size(); ++arg_idx) {
			size_t size = op_sizes[arg_idx];
			for (size_t byte_idx = 0; byte_idx < size; ++byte_idx) {
				std::cout <<
						std::hex << std::uppercase << std::setw(2) << std::setfill('0') <<
						static_cast<int>bytecode[offset + byte_idx];
			}
			std::cout << " ";
			offset += size;
		}
		std::cout << std::endl;
#endif

		result.bytecode.push_back(static_cast<uint8_t>(op_code));
		result.bytecode.insert(result.bytecode.end(), bytecode.begin(), bytecode.end());
	}
}

template <bool size_only>
inline void BytecodeGenerator<size_only>::adjust_pointers(const VariableExpression* dv, const VariableExpression* av, const VariableExpression* bv) {
	#define ADJUST_POINTER(m_pv, m_p, m_p_set) \
		if (m_pv) { \
			if (m_pv->var->scope->type == Scope::STATIC_CLASS) { \
				append(vm::Instruction::SET_##m_p##P_TO_STATIC, {}); \
				m_p_set = true; \
			} else if (m_pv->obj_expr) { \
				VarExprPtr obj_var_expr = std::dynamic_pointer_cast<VariableExpression>(m_pv->obj_expr); \
				vm::Value2 obj_var_idx = vm::Value2::from(obj_var_expr->var->get_index()); \
				\
				append( \
					vm::Instruction::SET_##m_p##P_TO_##m_p##_PTR, \
					{ \
						obj_var_idx.v[0].u8, obj_var_idx.v[1].u8 \
					} \
				); \
				m_p_set = true; \
			} \
		}
	
	ADJUST_POINTER(dv, D, dp_set)
	ADJUST_POINTER(av, A, ap_set)
	ADJUST_POINTER(bv, B, bp_set)

	#undef ADJUST_POINTER
}

template <bool size_only>
inline void BytecodeGenerator<size_only>::reset_pointers() {
	if (dp_set) {
		append(vm::Instruction::SET_DP_TO_FP, {});
		dp_set = false;
	}
	
	if (ap_set) {
		append(vm::Instruction::SET_AP_TO_FP, {});
		ap_set = false;
	}
	
	if (bp_set) {
		append(vm::Instruction::SET_BP_TO_FP, {});
		bp_set = false;
	}
}

// ================================================================================================
// Statements
// ================================================================================================
template <bool size_only>
inline void BytecodeGenerator<size_only>::generate_bytecode(const Statement &) {
	add_error();
}

template <bool size_only>
inline void BytecodeGenerator<size_only>::generate_bytecode(const DeclareStatement& stmt) {
	if (!stmt.variable_symbol->initial_value) return;

	VarExprPtr var_expr = std::make_shared<VariableExpression>(stmt.variable_symbol);

	if (stmt.variable_symbol->initial_value->get_kind() == Expression::STRING_LIT) {
		stmt.variable_symbol->initial_value->accept(*this, var_expr);
		return;
	}

	if (false) {
		auto native_type = std::dynamic_pointer_cast<NativeClassType>(stmt.variable_symbol->type);
		auto methods = native_type->cls.methods;

		bool found_copy_constructor = false;
		
		for (auto [name, f] : native_type->class_ptr->functions) {
			if (name != native_type->cls.name) continue;
			
			if (f->method_pair->arg_count != 1) continue;

			std::cout << "candidate: " << f->to_string() << std::endl;

			if (!native_type->is_cpp_type(f->method_pair->arg_types[0])) continue;

			std::cout << "call copy constructor" << std::endl;
			
			ExprVec args = { stmt.variable_symbol->initial_value };
			auto expr_left = std::make_shared<VariableExpression>(stmt.variable_symbol);
			auto call = std::make_shared<CallExpression>(f, args, expr_left);
			auto var_expr = std::make_shared<VariableExpression>(scope->variables.begin()->second);
			call->accept(*this, var_expr);

			found_copy_constructor = true;

			break;
		}

		if (!found_copy_constructor) {
			add_error();
		}
	} else {
		stmt.variable_symbol->initial_value->accept(*this, var_expr);
	}
}

template <bool size_only>
inline void BytecodeGenerator<size_only>::generate_bytecode(const AssignmentStatement &stmt) {
	auto left_var_expr = std::dynamic_pointer_cast<VariableExpression>(stmt.expr_left);

	if (left_var_expr->get_type()->get_kind() == Type::NATIVE_CLASS) {
		auto native_type = std::dynamic_pointer_cast<NativeClassType>(left_var_expr->get_type());
		auto methods = native_type->cls.methods;
		
		for (auto [name, f] : native_type->class_ptr->functions) {
			if (name != "operator=") continue;
			
			// TODO: check args
			
			ExprVec args = {stmt.expr_right};
			auto call = std::make_shared<CallExpression>(f, args, stmt.expr_left);
			auto var_expr = std::make_shared<VariableExpression>(scope->variables.begin()->second);
			call->accept(*this, var_expr);
			break;
		}
	} else {
		stmt.expr_right->accept(*this, left_var_expr);
	}
}

template <bool size_only>
inline void BytecodeGenerator<size_only>::generate_bytecode(const ConditionalJumpStatement &stmt) {
	if (!stmt.condition) {
		auto addr = vm::Value4::from(stmt.if_label->get_address());

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

	struct ConditionVisitor final : ExpressionVisitor {
		BytecodeGenerator& self;
		const ConditionalJumpStatement& stmt;

		ConditionVisitor(BytecodeGenerator& self, const ConditionalJumpStatement& stmt)
				: self(self), stmt(stmt) {}

		void visit(Expression&) override { self.add_error(); }
		void visit(VariableExpression& expr) override {
			auto src = vm::Value2::from(expr.var->get_index());
			auto if_addr = vm::Value4::from(stmt.if_label->get_address());

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
				auto else_addr = vm::Value4::from(stmt.else_label->get_address());
				
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
		void visit(UnaryExpression&) override { self.add_error(); }
		void visit(BinaryExpression& expr) override {
			auto left_var = std::dynamic_pointer_cast<VariableExpression>(expr.left)->var;
			auto right_var = std::dynamic_pointer_cast<VariableExpression>(expr.right)->var;
			auto left = vm::Value2::from(static_cast<uint16_t>(left_var->get_index()));
			auto right = vm::Value2::from(static_cast<uint16_t>(right_var->get_index()));
			auto if_addr = vm::Value4::from(stmt.if_label->get_address());

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
				auto else_addr = vm::Value4::from(stmt.else_label->get_address());
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
		void visit(LoadConstExpression&) override { self.add_error(); }
		void visit(CallExpression&) override { self.add_error(); }
		void visit(TupleExpression&) override { self.add_error(); }
		void visit(StringLiteralExpression&) override { self.add_error(); }


	};

	ConditionVisitor condition_visitor(*this, stmt);
	
	stmt.condition->accept(condition_visitor);
}

template <bool size_only>
inline void BytecodeGenerator<size_only>::generate_bytecode(const ReturnStatement&) {
	append(vm::Instruction::RET, {});
}

template <bool size_only>
inline void BytecodeGenerator<size_only>::generate_bytecode(const LabelStatement &stmt) {
	if constexpr(size_only) {
		if (scope->label_addresses.find(stmt.identifier) != scope->label_addresses.end()) {
			std::cerr << "ERROR, already has label " << stmt.to_string() << ": " << scope->label_addresses[stmt.identifier] << std::endl;
			exit(1);
		}
		scope->label_addresses[stmt.identifier] = result;
	}
}

template <bool size_only>
inline void BytecodeGenerator<size_only>::generate_bytecode(const ExpressionStatement &stmt) {
	auto var_expr = std::make_shared<VariableExpression>(scope->variables.begin()->second);
	stmt.expression->accept(*this, var_expr);
}

// ================================================================================================
// dest <- expr
// ================================================================================================
template <bool size_only>
inline void BytecodeGenerator<size_only>::generate_bytecode(const Expression&, VarExprPtr) {
	add_error();
}

template <bool size_only>
inline void BytecodeGenerator<size_only>::generate_bytecode(const VariableExpression &expr, VarExprPtr dest_var) {
	auto right_type = expr.get_type();
	
	if (!right_type->get_kind() == Type::KIND::PRIMITIVE) {
		add_error();
		return;
	}

	auto right_primitive_type = std::dynamic_pointer_cast<PrimitiveType>(right_type);

	adjust_pointers(dest_var.get(), &expr, nullptr);

	auto dest = vm::Value2::from(dest_var->var->get_index());
	auto left = vm::Value2::from(expr.var->get_index());
	//auto right = vm::Value4::from(0);

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
			//right.v[0].u8, right.v[1].u8, right.v[2].u8, right.v[3].u8,
			0, 0, 0, 0, 0, 0, 0, 0,
		}
	);

	reset_pointers();
}

template <bool size_only>
inline void BytecodeGenerator<size_only>::generate_bytecode(const UnaryExpression&, VarExprPtr) {
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
	
	auto dest = vm::Value2::from(dest_var->var->get_index());
	auto left = vm::Value2::from(left_op->var->get_index());
	auto right = vm::Value2::from(right_op->var->get_index());
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
	size_t size = dest_var->get_type()->get_size();
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

	adjust_pointers(dest_var.get(), nullptr, nullptr);

	std::vector<uint8_t> args;
	
	auto dest = vm::Value2::from(dest_var->var->get_index());
	args = { dest.v[0].u8, dest.v[1].u8 };
	
	vm::Value* v = expr.eval_constexpr();
	for (int i = 0; i < size; ++i) {
		args.push_back(v[i].u8);
	}

	append(op_code, std::move(args));

	reset_pointers();
}

template <bool size_only>
inline void BytecodeGenerator<size_only>::generate_bytecode(
		const CallExpression &expr,
		VarExprPtr dest_var) {
	if(!expr.f) {
		add_error();
		return;
	}

	// Allocate if shared
	if (dest_var->get_type()->is_pointer_type() && expr.f->is_constructor) {
		Type& type = dest_var->get_type()->get_inner_type();
		
		vm::Value2 shared_offset = vm::Value2::from(dest_var->var->get_index());
		vm::Value2 size = vm::Value2::from(type.get_size());

		append(
			vm::Instruction::MAKE_SHARED_RAW,
			{
				shared_offset.v[0].u8, shared_offset.v[1].u8,
				size.v[0].u8, size.v[1].u8
			}
		);
	}

	// call
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
		
		vm::Value2 obj_address = vm::Value2::from(obj_var_expr ? obj_var_expr->var->get_index() : 0);
		vm::Value2 args_address = vm::Value2::from(arg0 ? arg0->var->get_index() : 0);
		vm::Value2 ret_address = vm::Value2::from(dest_var->var->get_index());

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
		vm::Value2 ret_address = vm::Value2::from(dest_var->var->get_index());

		// push obj pointer to stack
		if (expr.obj_expr
				&& expr.obj_expr->get_type()->get_kind() != Type::STATIC_CLASS) {
			auto obj_var_expr = std::dynamic_pointer_cast<VariableExpression>(expr.obj_expr);
			//std::cout << "push this" << std::endl;
		} else if (expr.f->is_constructor) {
			vm::Value2 fp_address = vm::Value2::from(scope->size + 32);
			append(
				vm::Instruction::ADD_CONST_I64,
				{
					fp_address.v[0].u8, fp_address.v[1].u8,
					ret_address.v[0].u8, ret_address.v[1].u8,
					0, 0, 0, 0, 0, 0, 0, 0,
				}
			);
		}

		auto func_address = vm::Value4::from(expr.f->scope->starting_address);
		vm::Value2 args_address = vm::Value2::from(arg0 ? arg0->var->get_index() : 0);

		append(
			vm::Instruction::CALL,
			{
				func_address.v[0].u8, func_address.v[1].u8, func_address.v[2].u8, func_address.v[3].u8,
				args_address.v[0].u8, args_address.v[1].u8,
				ret_address.v[0].u8, ret_address.v[1].u8
			}
		);
	}
}

template <bool size_only>
inline void BytecodeGenerator<size_only>::generate_bytecode(const TupleExpression&, VarExprPtr) {
	add_error();
}

template <bool size_only>
inline void BytecodeGenerator<size_only>::generate_bytecode(const StringLiteralExpression &expr, VarExprPtr dest_var) {
	auto dest = vm::Value2::from(dest_var->var->get_index());
	vm::Value2 string_pos;
	if constexpr(size_only) {
		string_pos = vm::Value2::from(0);
	} else {
		string_pos = vm::Value2::from(result.const_memory.size());
		for (char c : expr.value) {
			result.const_memory.emplace_back(vm::Value::from(c));
		}
		result.const_memory.emplace_back(vm::Value::from<uint8_t>(0));
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
