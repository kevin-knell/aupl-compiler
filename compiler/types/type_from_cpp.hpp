#pragma once
#include <string>
#include <map>
#include "forward_declarations.hpp"


namespace cmp
{

std::map<std::string, TypePtr>& named_cpp_types();

TypePtr get_type_from_cpp(std::string name);

} // namespace cmp
