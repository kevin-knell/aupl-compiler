#pragma once
#include "type.hpp"
#include "instructions.hpp"

namespace cmp
{

struct PrimitiveType final : public Type {
public:
    static const TypePtr TYPE_I8;
    static const TypePtr TYPE_I16;
    static const TypePtr TYPE_I32;
    static const TypePtr TYPE_INT;

    static const TypePtr TYPE_U8;
    static const TypePtr TYPE_U16;
    static const TypePtr TYPE_U32;
    static const TypePtr TYPE_U64;

	static const TypePtr TYPE_F32;
    static const TypePtr TYPE_FLOAT;

    static const TypePtr TYPE_BOOL;
    static const TypePtr TYPE_VOID;

	const vm::BinType vm_bin_type;

    std::string to_string() const override;
    size_t get_size() const override;

	KIND get_kind() const override;

	bool is_cpp_type(const std::string& cpp_type) const override;

	std::string to_cpp_type_str() override;
private:
    const std::string name; // "int", "bool", etc.
    const size_t size;
	const std::vector<std::string> cpp_types;

    explicit PrimitiveType(const std::string& n, size_t s, vm::BinType bin_type, std::vector<std::string> cpp_types)
		: vm_bin_type(bin_type), name(n), size(s), cpp_types(cpp_types) {}

    friend TypePtr create(std::string n, size_t s, vm::BinType bin_type, std::vector<std::string> cpp_types);
};

} // namespace cmp