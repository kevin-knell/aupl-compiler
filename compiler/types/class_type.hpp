#pragma once
#include "type.hpp"

namespace cmp
{

struct ClassType final : public Type {
    std::string name;

    std::string to_string() const override { return name; }
    size_t get_size() const override { return 256; };

	KIND get_kind() const override { return CLASS; };

	bool is_cpp_type(const std::string& cpp_type) const override {
		std::cout << name << " ==? " << cpp_type << std::endl;
		return name == cpp_type
				|| ("const " + name) == cpp_type
				|| (name + "&") == cpp_type
				|| ("const " + name + "&") == cpp_type;
	}

    ClassType(const std::string& n) : name(n) {}

	std::string to_cpp_type_str() override { return name; };

	bool default_store_shared() const override { return true; }
};

} // namespace cmp
