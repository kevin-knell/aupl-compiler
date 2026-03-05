#include "primitive_type.hpp"
#include "color.hpp"
#include <algorithm>

namespace cmp
{

TypePtr create(std::string n, int s, vm::BinType bin_type, std::vector<std::string> cpp_types) {
    PrimitiveType result = PrimitiveType(n, s, bin_type, cpp_types);
    return std::make_shared<PrimitiveType>(result);
}

const TypePtr PrimitiveType::TYPE_I8  = create("i8", 1, vm::BinType::INT8, { "int8_t" });
const TypePtr PrimitiveType::TYPE_I16 = create("i16", 2, vm::BinType::INT16, { "int16_t" });
const TypePtr PrimitiveType::TYPE_I32 = create("i32", 4, vm::BinType::INT32, { "int32_t", "int" } );
const TypePtr PrimitiveType::TYPE_INT = create("int", 8, vm::BinType::INT64, { "int64_t", "long", "const int64_t" } );

const TypePtr PrimitiveType::TYPE_U8   = create("u8", 1, vm::BinType::UINT8, { "uint8_t" });
const TypePtr PrimitiveType::TYPE_U16  = create("u16", 2, vm::BinType::UINT16, { "uint16_t" });
const TypePtr PrimitiveType::TYPE_U32  = create("u32", 4, vm::BinType::UINT32, { "uint32_t" });
const TypePtr PrimitiveType::TYPE_U64  = create("u64", 8, vm::BinType::UINT64, { "uint64_t" });

const TypePtr PrimitiveType::TYPE_F32  = create("f32", 4, vm::BinType::FLOAT, { "float" });
const TypePtr PrimitiveType::TYPE_FLOAT = create("float", 8, vm::BinType::DOUBLE, { "double", "const double" });

const TypePtr PrimitiveType::TYPE_BOOL = create("bool", 1, vm::BinType::UINT8, { "bool" } );

const TypePtr PrimitiveType::TYPE_VOID = create("void", 0, vm::BinType::UINT8, { "void" } );

std::string PrimitiveType::to_string() const {
    return C_TYPE(name);
}

int PrimitiveType::get_size() const {
    return size;
}

Type::KIND PrimitiveType::get_kind() const {
	return PRIMITIVE;
}

bool PrimitiveType::is_cpp_type(const std::string &cpp_type) const {
	return std::find(cpp_types.begin(), cpp_types.end(), cpp_type) != cpp_types.end();
}

} // namespace cmp

static_assert(sizeof(int) == 4);
static_assert(sizeof(long) == 8);
