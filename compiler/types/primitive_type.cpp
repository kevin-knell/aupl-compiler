#include "primitive_type.hpp"

#include "color.hpp"

namespace cmp
{

TypePtr create(std::string n, int s) {
    PrimitiveType result = PrimitiveType(n, s);
    return std::make_shared<PrimitiveType>(result);
}

const TypePtr PrimitiveType::TYPE_I8  = create("i8", 1);
const TypePtr PrimitiveType::TYPE_I16 = create("i16", 2);
const TypePtr PrimitiveType::TYPE_I32 = create("i32", 4);
const TypePtr PrimitiveType::TYPE_INT = create("int", 8);

const TypePtr PrimitiveType::TYPE_U8   = create("u8", 1);
const TypePtr PrimitiveType::TYPE_U16  = create("u16", 2);
const TypePtr PrimitiveType::TYPE_U32  = create("u32", 4);
const TypePtr PrimitiveType::TYPE_U64  = create("u64", 8);

const TypePtr PrimitiveType::TYPE_F32  = create("f32", 4);
const TypePtr PrimitiveType::TYPE_FLOAT = create("float", 8);

const TypePtr PrimitiveType::TYPE_BOOL = create("bool", 1);

const TypePtr PrimitiveType::TYPE_VOID = create("void", 0);

std::string PrimitiveType::to_string() const {
    return C_TYPE(name);
}

int PrimitiveType::get_size() const {
    return size;
}

} // namespace cmp
