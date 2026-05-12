#pragma once
#include "type.hpp"

namespace vm
{

class ClassBind;

} // namespace vm


namespace cmp
{

struct NativeClassType final : public Type {
	const vm::ClassBind& cls;
	ClassPtr class_ptr;

	NativeClassType(const vm::ClassBind& cls);
    std::string to_string() const override;
    size_t get_size() const override;
	FuncVec functions;

	KIND get_kind() const override;

	bool is_cpp_type(const std::string& cpp_type) const override;

	std::string to_cpp_type_str() override;

	bool default_store_shared() const override;
};

} // namespace cmp