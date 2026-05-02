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

#include "label.hpp"
#include <assert.h>



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

		append(hpp_classes, "class " + cn + " {");
		append(hpp_classes, "public:");

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

	if (stmt.variable_symbol->type->get_kind() == Type::SHARED) {
		ss << " = ";
		ss << stmt.variable_symbol->type->to_cpp_type_str();
		ss << "::make(";
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
	cpp_classes << " = ";
	stmt.expr_right->accept(*this);
	cpp_classes << ";\n";
}

void cmp::CppCodeGenerator::visit(ConditionalJumpStatement &stmt) {
	if (stmt.condition) {
		// block start
		cpp_classes << make_indented_stringstream().str();
		
		if (stmt.cj_kind == CJ_KIND::IF) {
			cpp_classes << "if (";
		} else if (stmt.cj_kind == CJ_KIND::WHILE) {
			cpp_classes << "while (";
		} else {
			throw std::runtime_error("invalid jump with condition!");
		}

		stmt.condition->accept(*this);
		cpp_classes << ") {\n";

		// block content
		scope = stmt.if_label->scope;
		
		iterate_scope();

		ScopePtr upper_scope = scope->upper_scope.lock();
		assert(upper_scope);
		scope = upper_scope;

		// block end
		cpp_classes << make_indented_stringstream().str();
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
	stmt.return_value->accept(*this);
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

	if (expr.get_type()->is_pointer_type()) {
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

void cmp::CppCodeGenerator::visit(StringLiteralExpression &expr) {
	cpp_classes << "\"";
	cpp_classes << expr.value;
	cpp_classes << "\"";
}
