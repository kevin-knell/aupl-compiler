#include "variable_symbol.hpp"

#include "color.hpp"

#define TAG(s, b) std::string(b ? s : "")

namespace cmp
{

std::string VariableSymbol::to_string() const {
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

} // namespace cmp

#undef TAG