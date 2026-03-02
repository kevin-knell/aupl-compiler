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

		INVALID
	};

    virtual ~Type() = default;
    virtual std::string to_string() const = 0;
    virtual int get_size() const = 0;
	virtual KIND get_kind() const = 0;
};

using TypePtr = std::shared_ptr<Type>;

}