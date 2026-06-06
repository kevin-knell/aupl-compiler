#pragma once
#include "type.hpp"
#include "class_db.hpp"

namespace cmp
{

struct ClassType final : public Type {
    std::string name;
	const vm::ClassBind* class_bind;
	ClassPtr class_ptr;

	ClassType() = delete;

    ClassType(const std::string& n) : name(n), class_bind(nullptr) {}

	ClassType(const vm::ClassBind& class_bind);

    std::string to_string() const override { return name; }
    size_t get_size() const override { return 256; };

	KIND get_kind() const override { return CLASS; };

	bool is_cpp_type(const std::string& cpp_type) const override;
	
	bool is_convertable_to_cpp_type(const std::string&) const override { return false; }

	std::string to_cpp_type_str() override { return name; };

	bool default_store_shared() const override;
};

} // namespace cmp
