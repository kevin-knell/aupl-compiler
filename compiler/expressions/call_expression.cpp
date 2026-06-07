#include "call_expression.hpp"
#include "type.hpp"
#include "function_symbol.hpp"
#include "invalid_type.hpp"
#include "class_symbol.hpp"
#include "text_color.hpp"
#include "symbol_table.hpp"
#include "variable_expression.hpp"
#include "static_class_type.hpp"
#include "bytecode_generator.hpp"
#include "class_type.hpp"
#include "forward_declarations.hpp"
#include "pointer_type.hpp"

#include "compiler_error.hpp"

namespace cmp {

std::string CallExpression::to_string() const {
    std::string result;

	result += obj_expr ? obj_expr->to_string() + "." : "";
	
	result += f ? C_IDENTIFIER(f->name) : C_UNRES(name);

    result += "(";

    for (auto it = arguments.begin(); it != arguments.end(); ++it) {
        if (it != arguments.begin()) {
            result += ", ";
        }

        result += it->get()->to_string();
    }

    result += ")";

    return result;
}

int CallExpression::get_level() {
	int super_level = Expression::get_level();
	if (super_level == 0) {
		return 1;
	} else {
		return super_level;
	}
}

bool CallExpression::is_unresolved_symbol() const {
    return !f || (obj_expr && obj_expr->is_unresolved_symbol());
}

std::vector<ExprPtr *> CallExpression::get_expressions() {
    std::vector<ExprPtr *> result;

    for (auto& expr : arguments) {
        result.push_back(&expr);
    }

    return result;
}

bool CallExpression::is_pure() const {
    return f->is_pure;
}

TypePtr CallExpression::get_type() const {
    if (f && f->return_type) {
		if (f->is_constructor) {
			return std::make_shared<ClassType>(f->name);
		} else {
			return f->return_type;
		}
	}
	
	return InvalidType::make("unknown call: " + name);
}

}