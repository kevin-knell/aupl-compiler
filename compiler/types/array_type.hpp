#pragma once
#include "type.hpp"
#include "expression.hpp"

namespace cmp
{

struct ArrayType : public Type {
	TypePtr base_type;
	ExprPtr size_expr;
	bool is_const;

    std::string to_string() const override;
    
	int get_size() const override { return 1; }

	KIND get_kind() const override;

	bool is_cpp_type(const std::string&) const override { return false; }

	ArrayType(TypePtr base_type, ExprPtr size_expr, bool is_const)
			: base_type(base_type), size_expr(size_expr), is_const(is_const) {};
};

}