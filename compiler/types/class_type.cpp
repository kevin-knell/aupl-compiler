#include "class_type.hpp"
#include "class_db.hpp"
#include "class_symbol.hpp"

namespace cmp {

ClassType::ClassType(const vm::ClassBind& class_bind) : name(class_bind.name), class_bind(&class_bind) {}

std::string ClassType::to_string() const {
	if (!!class_bind) {
		if (class_bind->is_trivial) {
			return C_TYPE(name);
		} else {
			return C_KEYWORD(name);
		}
	} else {
		return name;
	}
}

bool ClassType::is_cpp_type(const std::string &cpp_type) const {
	//std::cout << name << " ==? " << cpp_type << std::endl;
	return name == cpp_type
			|| ("const " + name) == cpp_type
			|| (name + "&") == cpp_type
			|| ("const " + name + "&") == cpp_type
			|| ("Shared<" + name + ">") == cpp_type
			|| (
				class_ptr->parent
				&& class_ptr->parent->type->is_cpp_type(cpp_type)
			);
}

bool ClassType::is_convertable_to_cpp_type(const std::string &) const {
	return false;
}

bool ClassType::default_store_shared() const {
	if (!class_bind) return true;
	return class_bind->is_object;
}

}