#include "class_type.hpp"
#include "class_db.hpp"

namespace cmp {

ClassType::ClassType(const vm::ClassBind& class_bind) : name(class_bind.name), class_bind(&class_bind) {}

bool ClassType::is_cpp_type(const std::string &cpp_type) const {
	//std::cout << name << " ==? " << cpp_type << std::endl;
	return name == cpp_type
			|| ("const " + name) == cpp_type
			|| (name + "&") == cpp_type
			|| ("const " + name + "&") == cpp_type
			|| ("Shared<" + name + ">") == cpp_type;
}

bool ClassType::default_store_shared() const {
	return !class_bind
			|| class_bind->is_object;
}

}