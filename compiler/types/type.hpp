#pragma once
#include <string>
#include <memory>
#include "forward_declarations.hpp"

namespace cmp {

struct Type {
	enum KIND {
		PRIMITIVE,

		CLASS,
		STATIC_CLASS,
		NATIVE_CLASS,

		TUPLE,
		ARRAY,

		POINTER,
		SHARED,

		INVALID
	};

    virtual ~Type() = default;
    virtual std::string to_string() const = 0;
    virtual int get_size() const = 0;
	virtual KIND get_kind() const = 0;
	virtual bool is_cpp_type(const std::string& cpp_type) const = 0;

	virtual bool is_pointer_type() { return false; }
	virtual Type& get_inner_type() { return *this; }
};

using TypePtr = std::shared_ptr<Type>;

}