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
	auto sc = scope.lock();
	
	COMPILER_ASSERT(sc, to_string() + " has no scope");
	COMPILER_ASSERT(
		sc->has(name),
		"scope of " + to_string() + " does not have itself:\n" +
		sc->get_full_name() + "\n" +
		sc->structure_to_string()
	);
	return Scope::get_variable_index(sc, name);
}

} // namespace cmp

#undef TAG