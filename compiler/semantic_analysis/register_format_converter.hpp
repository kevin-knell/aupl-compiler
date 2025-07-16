#pragma once
#include "register_format_info.hpp"

namespace cmp {

class RegisterFormatConverter {
private:
    static void convert_to_register_format(RegisterFormatInfo& rfi, ExprPtr expr, bool is_volatile);
public:
    static void convert_to_register_format(SymbolTable& symbol_table);
    RegisterFormatConverter() = delete;
};

} // namespace cmp
