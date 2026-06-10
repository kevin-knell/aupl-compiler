#include "variable_symbol.hpp"

#include "text_color.hpp"
#include "scope.hpp"
#include "type_from_cpp.hpp"
#include "compiler_error.hpp"

#define TAG(s, b) std::string(b ? s : "")

namespace cmp
{

VariableSymbol::VariableSymbol(Private, vm::VariableBind &v)
		: type(get_type_from_cpp(v.type)), name(v.name), var_bind(&v) {}

std::string VariableSymbol::to_string() const {
	if (!type) return "UnknownType " + name;

    return C_KEYWORD(
                TAG("public ", is_public) +
                TAG("static ", is_static) +
                TAG("const ", is_const)
            ) +
            type->to_string() + " " +
            name_to_string();
}

std::string VariableSymbol::name_to_string() const {
    return name.front() == '%' ? C_TEMP(name) : C_IDENTIFIER(name);
}

size_t VariableSymbol::get_index() const {
	COMPILER_ASSERT(scope, "");
	COMPILER_ASSERT(scope->has(name), "");
	return Scope::get_variable_index(scope, name);
}

} // namespace cmp

#undef TAG