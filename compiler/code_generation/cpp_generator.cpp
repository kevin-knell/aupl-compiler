#include "cpp_generator.hpp"

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
#include "tuple_expression.hpp"
#include "index_expression.hpp"

#include "label.hpp"
#include "compiler_error.hpp"



#ifdef CPP_CG_DEBUG_VERBOSE
#define CPP_CG_DEBUG
#define CPP_CG_DEBUG_PRINT(m_text) std::cout << m_text << std::endl
#define CPP_CG_DEBUG_PRINT_V(m_text) BCG_DEBUG_PRINT(m_text)
#define CPP_CG_DEBUG_PRINT_NV(m_text)
#elifdef CPP_CG_DEBUG
#define CPP_CG_DEBUG_PRINT(m_text) std::cout << m_text << std::endl
#define CPP_CG_DEBUG_PRINT_V(m_text)
#define CPP_CG_DEBUG_PRINT_NV(m_text) BCG_DEBUG_PRINT(m_text)
#else
#define CPP_CG_DEBUG_PRINT(m_text)
#define CPP_CG_DEBUG_PRINT_V(m_text)
#define CPP_CG_DEBUG_PRINT_NV(m_text)
#endif

namespace {
	int indentation_level = 0;

	std::stringstream make_indented_stringstream() {
		std::stringstream ss;
		
		for (int i = 0; i < indentation_level; ++i) {
			ss << "\t";
		}

		return ss;
	}
}

#define append(m_ss, m_str) do { m_ss << m_str << "\n"; } while(0)


void cmp::CppCodeGenerator::generate_cpp_code(std::ofstream &hpp_file, std::ofstream &cpp_file) {
	append(hpp_includes, "// include");
	append(hpp_forward_declarations, "\n\n\n// forward declarations");
	append(hpp_forward_declarations, "using namespace auplib;");
	append(hpp_classes, "\n\n\n// classes");
	append(hpp_global_functions, "\n\n\n// global functions");

	append(cpp_includes, "// include");
	append(cpp_classes, "\n\n\n// classes");


	append(hpp_includes, "#pragma once");
	append(hpp_includes, "#include \"string.hpp\"");
	append(hpp_includes, "#include \"shared.hpp\"");
	append(hpp_includes, "#include \"console.hpp\"");
	append(hpp_includes, "#include \"math.hpp\"");
	append(hpp_includes, "#include \"file.hpp\"");
	append(hpp_includes, "#include \"window.hpp\"");
	append(hpp_includes, "#include \"color_rect.hpp\"");
	append(hpp_includes, "#include \"color.hpp\"");
	append(hpp_includes, "#include \"vec2.hpp\"");
	append(hpp_includes, "#include \"renderer.hpp\"");
	append(hpp_includes, "#include \"scene.hpp\"");
	append(hpp_includes, "#include \"node.hpp\"");
	append(hpp_includes, "#include \"time.hpp\"");
	append(hpp_includes, "#include \"input.hpp\"");

	append(cpp_includes, "#include \"output.hpp\"");

	// classes
    for (auto [cn, cls] : symbol_table.classes) {
		if (cls->native_class_bind) {
			for (auto method_pair : cls->native_class_bind->methods) {
				if (method_pair.is_global && !method_pair.is_constructor) {
					append(hpp_global_functions, "inline " + method_pair.signature + " {");
					hpp_global_functions << "\treturn " + cn + "::" + method_pair.name + "(";

					for (size_t i = 0; i < method_pair.arg_count; ++i) {
						if (i != 0) {
							hpp_global_functions << ", ";
						}

						hpp_global_functions << method_pair.arg_names[i];
					}

					hpp_global_functions << ");\n";
					append(hpp_global_functions, "}");
				}
			}
			continue;
		}
		
		CPP_CG_DEBUG_PRINT(cn);

		current_class = cls;

		append(hpp_forward_declarations, "class " + cn + ";");

		if (cls->parent) {
			append(hpp_classes, "class " + cn + " : public " + cls->parent->name + " {");
		} else {
			append(hpp_classes, "class " + cn + " {");
		}
		append(hpp_classes, "public:");

		auto static_class_scope = current_class->static_scope;

		for (auto [vn, var] : static_class_scope->variables) {
			hpp_classes << "\tstatic ";
			hpp_classes << var->type->to_cpp_type_str();
			hpp_classes << " ";
			hpp_classes << vn;
			// todo: initial value
			hpp_classes << ";\n";

			cpp_classes << var->type->to_cpp_type_str();
			cpp_classes << " ";
			cpp_classes << cn;
			cpp_classes << "::";
			cpp_classes << vn;
			// todo: initial value
			cpp_classes << ";\n";
		}



		auto class_scope = current_class->scope;

		for (auto [vn, var] : class_scope->variables) {
			hpp_classes << "\t";
			hpp_classes << var->type->to_cpp_type_str();
			hpp_classes << " ";
			hpp_classes << vn;
			// todo: initial value
			hpp_classes << ";\n";
		}

		// functions
        for (auto [fn, f] : cls->functions) {
			CPP_CG_DEBUG_PRINT("\n" << f->head_to_string());

			if (fn == "main") {
				append(cpp_main, "int main() {");
				append(cpp_main, "\t" + cn + "::main();");
				append(cpp_main, "}");
			}

			append(hpp_classes, "\t" + f->to_cpp_string_prototype());
			append(cpp_classes, f->to_cpp_string(cn) + " {");

			current_function = f;

			// scopes
			scope = f->scope;

			iterate_scope();

			append(cpp_classes, "}");
        }

		append(hpp_classes, "};");
		append(hpp_classes, "");
		
		append(cpp_classes, "");
    }

	hpp_file << hpp_includes.str() << hpp_forward_declarations.str() << hpp_classes.str() << hpp_global_functions.str();
	cpp_file << cpp_includes.str() << cpp_classes.str() << cpp_main.str();
}

void cmp::CppCodeGenerator::iterate_scope() {
	++indentation_level;
	
	for (auto stmt : scope->body) {
		stmt->accept(*this);
	}

	--indentation_level;
};

// ================================================================================================
// Statements
// ================================================================================================
void cmp::CppCodeGenerator::visit(DeclareStatement &stmt) {
	std::stringstream ss = make_indented_stringstream();
	
	ss << stmt.variable_symbol->type->to_cpp_type_str();
	
	ss << " ";
	auto name = stmt.variable_symbol->name;
	if (name.starts_with("%")) {
		name.replace(0, 1, 1, '_');
	}

	ss << name;

	bool needs_make_shared = false;

	if (stmt.variable_symbol->type->get_kind() == Type::SHARED
			&& stmt.variable_symbol->initial_value->get_kind() == Expression::CALL) {
		std::shared_ptr<CallExpression> call_expr = std::static_pointer_cast<CallExpression>(stmt.variable_symbol->initial_value);
		
		needs_make_shared = call_expr->f->is_constructor;
	}

	if (needs_make_shared) {
		ss << " = ";
		ss << stmt.variable_symbol->type->to_cpp_type_str();
		ss << "(new ";
		cpp_classes << ss.str();
		stmt.variable_symbol->initial_value->accept(*this);
		cpp_classes << ")";
	} else {
		cpp_classes << ss.str();

		if (stmt.variable_symbol->initial_value) {
			cpp_classes << " = ";
			stmt.variable_symbol->initial_value->accept(*this);
		}
	}
	cpp_classes << ";\n";
}

void cmp::CppCodeGenerator::visit(AssignmentStatement &stmt) {
	cpp_classes << make_indented_stringstream().str();
	stmt.expr_left->accept(*this);

	std::stringstream ss = make_indented_stringstream();

	bool needs_make_shared = false;

	if (stmt.expr_left->get_type()->get_kind() == Type::SHARED
			&& stmt.expr_right->get_kind() == Expression::CALL) {
		std::shared_ptr<CallExpression> call_expr = std::static_pointer_cast<CallExpression>(stmt.expr_right);
		
		needs_make_shared = call_expr->f->is_constructor;
	}

	if (needs_make_shared) {
		ss << " = ";
		ss << stmt.expr_left->get_type()->to_cpp_type_str();
		ss << "(new ";
		cpp_classes << ss.str();
		stmt.expr_right->accept(*this);
		cpp_classes << ")";
	} else {
		cpp_classes << " = ";
		stmt.expr_right->accept(*this);
	}
	cpp_classes << ";\n";
}

void cmp::CppCodeGenerator::visit(ConditionalJumpStatement &stmt) {
	if (stmt.condition) {
		// block start
		cpp_classes << make_indented_stringstream().str();
		
		if (stmt.cj_kind == CJ_KIND::IF) {
			cpp_classes << "if (";
			stmt.condition->accept(*this);
			cpp_classes << ") {\n";
		} else if (stmt.cj_kind == CJ_KIND::WHILE) {
			cpp_classes << "while (true) { ";
			cpp_classes << "if (not (";
			stmt.condition->accept(*this);
			cpp_classes << ")) break;\n";
		} else if (stmt.cj_kind == CJ_KIND::FOR) {
			cpp_classes << "for (";
			std::shared_ptr<BinaryExpression> compare_expr = std::dynamic_pointer_cast<BinaryExpression>(stmt.condition);
			VarExprPtr it_var_expr = std::dynamic_pointer_cast<VariableExpression>(compare_expr->left);
			cpp_classes << it_var_expr->get_type()->to_cpp_type_str() << " ";
			cpp_classes << it_var_expr->name;
			cpp_classes << " = 0; ";
			cpp_classes << it_var_expr->name;
			cpp_classes << " < ";
			compare_expr->right->accept(*this);
			cpp_classes << "; ";
			cpp_classes << ") {\n";
		} else {
			throw std::runtime_error("invalid jump with condition!");
		}

		// block content
		scope = stmt.if_label->scope;
		
		iterate_scope();

		ScopePtr upper_scope = scope->upper_scope.lock();
		COMPILER_ASSERT(upper_scope, "");
		scope = upper_scope;

		// block end
		cpp_classes << make_indented_stringstream().str();

		if (stmt.cj_kind == CJ_KIND::WHILE) {
			cpp_classes << "if (not (";
			stmt.condition->accept(*this);
			cpp_classes << ")) break;\n";
		}

		cpp_classes << "}\n";
	} else {
		cpp_classes << make_indented_stringstream().str();
		cpp_classes << "// goto ";
		cpp_classes << stmt.if_label->identifier;
		cpp_classes << "\n";
	}
}

void cmp::CppCodeGenerator::visit(ReturnStatement &stmt) {
	cpp_classes << make_indented_stringstream().str();
	cpp_classes << "return ";
	if (stmt.return_value) stmt.return_value->accept(*this);
	cpp_classes << ";\n";
}

void cmp::CppCodeGenerator::visit(LabelStatement &stmt) {
	cpp_classes << make_indented_stringstream().str();
	cpp_classes << "// @";
	cpp_classes << stmt.identifier;
	cpp_classes << "\n";
}

void cmp::CppCodeGenerator::visit(ExpressionStatement &stmt) {
	cpp_classes << make_indented_stringstream().str();

	stmt.expression->accept(*this);

	append(cpp_classes, ";");
}

// ================================================================================================
// Expressions
// ================================================================================================
void cmp::CppCodeGenerator::visit(VariableExpression &expr) {
	auto name = expr.name;
	
	if (name.starts_with("%")) {
		name.replace(0, 1, 1, '_');
	}

	if (expr.obj_expr) {
		expr.obj_expr->accept(*this);
		cpp_classes << ".";
	}

	if (expr.must_be_dereferenced) {
		cpp_classes << "(*";
		cpp_classes << name;
		cpp_classes << ")";
	} else {
		cpp_classes << name;
	}
}

void cmp::CppCodeGenerator::visit(UnaryExpression &expr) {
	cpp_classes << "(";
	cpp_classes << expr.operator_to_string();
	cpp_classes << " ";
	expr.expr->accept(*this);
	cpp_classes << ")";
}

void cmp::CppCodeGenerator::visit(BinaryExpression &expr) {
	cpp_classes << "(";
	expr.left->accept(*this);
	
	cpp_classes << " ";
	cpp_classes << expr.operator_to_string();
	cpp_classes << " ";
	
	expr.right->accept(*this);
	cpp_classes << ")";
}

void cmp::CppCodeGenerator::visit(LoadConstExpression &expr) {
	cpp_classes << expr.value_to_string();
}

void cmp::CppCodeGenerator::visit(CallExpression &expr)
{
	std::stringstream ss;

	if (expr.obj_expr) {
		expr.obj_expr->accept(*this);
		
		if (expr.obj_expr->get_kind() == Expression::VARIABLE) {
			auto obj_expr_type = expr.obj_expr->get_type();
			if (obj_expr_type->get_kind() == Type::STATIC_CLASS) {
				ss << "::";
			} else {
				ss << ".";
			}
		} else if (expr.obj_expr->get_kind() == Expression::CALL) {
			ss << ".";
		} else {
			throw std::runtime_error("Invalid kind for obj_expression: " + expr.obj_expr->to_string());
		}
	}

	ss << expr.name;
	ss << "(";

	cpp_classes << ss.str();

	for (size_t i = 0; i < expr.arguments.size(); ++i) {
		if (i != 0) {
			cpp_classes << ", ";
		}
		ExprPtr arg = expr.arguments[i];
		arg->accept(*this);
	}

	cpp_classes << ")";
}

void cmp::CppCodeGenerator::visit(TupleExpression &expr) {
	cpp_classes << "{";

	for (ExprPtr e : expr.expressions) {
		e->accept(*this);
		cpp_classes << ",";
	}

	cpp_classes << "}";
}

void cmp::CppCodeGenerator::visit(StringLiteralExpression &expr) {
	cpp_classes << "\"";
	cpp_classes << expr.value;
	cpp_classes << "\"";
}

void cmp::CppCodeGenerator::visit(IndexExpression &expr) {
	expr.left_expr->accept(*this);
	cpp_classes << "[";
	expr.index_expr->accept(*this);
	cpp_classes << "]";
}
