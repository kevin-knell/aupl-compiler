#include "type_from_cpp.hpp"
#include "primitive_type.hpp"
#include "invalid_type.hpp"

namespace cmp
{

std::map<std::string, TypePtr>& named_cpp_types() {
    static std::map<std::string, TypePtr> instance;
    return instance;
}

TypePtr get_type_from_cpp(std::string name) {
	if (named_cpp_types().contains(name)) {
		return named_cpp_types()[name];
	}
	
	return std::make_shared<InvalidType>("(unknown C++ type)" + name);
}

} // namespace cmp