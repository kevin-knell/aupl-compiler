#pragma once
#include "type.hpp"

namespace vm
{

class ClassBind;

} // namespace vm


namespace cmp
{

struct NativeClassType : public Type {
	const vm::ClassBind& cls;

	NativeClassType(const vm::ClassBind& cls);
    std::string to_string() const override;
    int get_size() const override;
	FuncVec functions;
};

} // namespace cmp