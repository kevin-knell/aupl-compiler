#pragma once
#include "type.hpp"

namespace cmp
{

struct PrimitiveType : public Type {
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

    std::string to_string() const override;
    int get_size() const override;
private:
    const std::string name; // "int", "bool", etc.
    const int size;

    explicit PrimitiveType(const std::string& n, int s) : name(n), size(s) {}

    friend TypePtr create(std::string n, int s);
};

} // namespace cmp