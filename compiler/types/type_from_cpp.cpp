#include "type_from_cpp.hpp"
#include "primitive_type.hpp"
#include "invalid_type.hpp"
#include "shared_type.hpp"
#include "compiler_error.hpp"
#include "array_type.hpp"

namespace cmp
{

std::map<std::string, TypePtr>& named_cpp_types() {
	static std::map<std::string, TypePtr> instance;
	return instance;
}

TypePtr get_type_from_cpp(std::string name) {
	if (name.contains('<')) {
		std::string name_without_template;
		size_t template_start = name.find('<');
		name_without_template = name.substr(0, template_start);
		
		size_t inner_type_size = name.size() - name_without_template.size() - 2;
		std::string inner_type_name = name.substr(template_start + 1, inner_type_size);
		std::cout << inner_type_name << std::endl;
		TypePtr inner_type = get_type_from_cpp(inner_type_name);

		if (name_without_template == "Shared") {
			std::shared_ptr<SharedType> shared_type = std::make_shared<SharedType>(inner_type);
			return shared_type;
		} else if (name_without_template == "List") {
			std::shared_ptr<ArrayType> array_type = std::make_shared<ArrayType>(inner_type, nullptr, false);
			return array_type;
		} else {
			COMPILER_ERR("generics are not yet fully supported: " + name);
		}
	}
	
	if (named_cpp_types().contains(name)) {
		return named_cpp_types()[name];
	}
	
	return std::make_shared<InvalidType>("(unknown C++ type)" + name);
}

} // namespace cmp