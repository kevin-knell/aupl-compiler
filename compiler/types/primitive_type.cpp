#include "primitive_type.hpp"
#include "color.hpp"

namespace cmp
{

TypePtr create(std::string n, int s, vm::BinType bin_type) {
    PrimitiveType result = PrimitiveType(n, s, bin_type);
    return std::make_shared<PrimitiveType>(result);
}

const TypePtr PrimitiveType::TYPE_I8  = create("i8", 1, vm::BinType::INT8);
const TypePtr PrimitiveType::TYPE_I16 = create("i16", 2, vm::BinType::INT16);
const TypePtr PrimitiveType::TYPE_I32 = create("i32", 4, vm::BinType::INT32);
const TypePtr PrimitiveType::TYPE_INT = create("int", 8, vm::BinType::INT64);

const TypePtr PrimitiveType::TYPE_U8   = create("u8", 1, vm::BinType::UINT8);
const TypePtr PrimitiveType::TYPE_U16  = create("u16", 2, vm::BinType::UINT16);
const TypePtr PrimitiveType::TYPE_U32  = create("u32", 4, vm::BinType::UINT32);
const TypePtr PrimitiveType::TYPE_U64  = create("u64", 8, vm::BinType::UINT64);

const TypePtr PrimitiveType::TYPE_F32  = create("f32", 4, vm::BinType::FLOAT);
const TypePtr PrimitiveType::TYPE_FLOAT = create("float", 8, vm::BinType::DOUBLE);

const TypePtr PrimitiveType::TYPE_BOOL = create("bool", 1, vm::BinType::UINT8);

const TypePtr PrimitiveType::TYPE_VOID = create("void", 0, vm::BinType::UINT8);

std::string PrimitiveType::to_string() const {
    return C_TYPE(name);
}

int PrimitiveType::get_size() const {
    return size;
}

Type::KIND PrimitiveType::get_kind() const {
	return PRIMITIVE;
}

} // namespace cmp
