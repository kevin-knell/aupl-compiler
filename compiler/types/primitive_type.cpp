#include "primitive_type.hpp"
#include "text_color.hpp"
#include <algorithm>
#include "type_from_cpp.hpp"
#include <cstring>

namespace cmp
{

TypePtr create(
		std::string n,
		size_t s,
		vm::BinType bin_type,
		std::vector<std::string> cpp_types_synonymous,
		std::vector<std::string> cpp_types_convertable) {
    PrimitiveType type = PrimitiveType(n, s, bin_type, cpp_types_synonymous, cpp_types_convertable);
    auto result = std::make_shared<PrimitiveType>(type);

	for (auto s : cpp_types_synonymous) {
		named_cpp_types()[s] = result;
		named_cpp_types()["const " + s] = result;
	}

	return result;
}

#define CPP_TYPES_I8 "int8_t", "signed char"
#define CPP_TYPES_I16 "int16_t", "short", "signed short"
#define CPP_TYPES_I32 "int32_t", "int", "signed int"
#define CPP_TYPES_I64 "int64_t", "long", "signed long"

#define CPP_TYPES_U8 "uint8_t", "unsigned char"
#define CPP_TYPES_U16 "uint16_t", "unsigned short"
#define CPP_TYPES_U32 "uint32_t", "unsigned int"
#define CPP_TYPES_U64 "uint64_t", "unsigned long"

#define CPP_TYPES_ALL_INTEGER \
	CPP_TYPES_I8, \
	CPP_TYPES_I16, \
	CPP_TYPES_I32, \
	CPP_TYPES_I64, \
	CPP_TYPES_U8, \
	CPP_TYPES_U16, \
	CPP_TYPES_U32, \
	CPP_TYPES_U64


#define CPP_TYPES_F32 "float"
#define CPP_TYPES_F64 "double"

#define CPP_TYPES_BOOL "bool"

#define CPP_TYPES_VOID "void"

#define CPP_TYPES_I8_CON CPP_TYPES_I8, CPP_TYPES_ALL_INTEGER
#define CPP_TYPES_I16_CON CPP_TYPES_I16, CPP_TYPES_I8_CON
#define CPP_TYPES_I32_CON CPP_TYPES_I32, CPP_TYPES_I16_CON
#define CPP_TYPES_I64_CON CPP_TYPES_I64, CPP_TYPES_I32_CON

#define CPP_TYPES_U8_CON CPP_TYPES_U8, CPP_TYPES_I8, CPP_TYPES_ALL_INTEGER
#define CPP_TYPES_U16_CON CPP_TYPES_U16, CPP_TYPES_I16_CON, CPP_TYPES_U8_CON
#define CPP_TYPES_U32_CON CPP_TYPES_U32, CPP_TYPES_I32_CON, CPP_TYPES_U16_CON
#define CPP_TYPES_U64_CON CPP_TYPES_U64, CPP_TYPES_I64_CON, CPP_TYPES_U32_CON

#define CPP_TYPES_F32_CON CPP_TYPES_F32, CPP_TYPES_ALL_INTEGER
#define CPP_TYPES_F64_CON CPP_TYPES_F64, CPP_TYPES_F32_CON

#define CPP_TYPES_BOOL_CON CPP_TYPES_BOOL

#define CPP_TYPES_VOID_CON CPP_TYPES_VOID

const TypePtr PrimitiveType::TYPE_I8  = create("i8", 1, vm::BinType::INT8, { CPP_TYPES_I8 }, { CPP_TYPES_I8_CON } );
const TypePtr PrimitiveType::TYPE_I16 = create("i16", 2, vm::BinType::INT16, { CPP_TYPES_I16 }, { CPP_TYPES_I16_CON } );
const TypePtr PrimitiveType::TYPE_I32 = create("i32", 4, vm::BinType::INT32, { CPP_TYPES_I32 }, { CPP_TYPES_I32_CON } );
const TypePtr PrimitiveType::TYPE_INT = create("int", 8, vm::BinType::INT64, { CPP_TYPES_I64 }, { CPP_TYPES_I64_CON } );

const TypePtr PrimitiveType::TYPE_U8   = create("u8", 1, vm::BinType::UINT8, { CPP_TYPES_U8 }, { CPP_TYPES_U8_CON } );
const TypePtr PrimitiveType::TYPE_U16  = create("u16", 2, vm::BinType::UINT16, { CPP_TYPES_U16 }, { CPP_TYPES_U16_CON } );
const TypePtr PrimitiveType::TYPE_U32  = create("u32", 4, vm::BinType::UINT32, { CPP_TYPES_U32 }, { CPP_TYPES_U32_CON } );
const TypePtr PrimitiveType::TYPE_U64  = create("u64", 8, vm::BinType::UINT64, { CPP_TYPES_U64 }, { CPP_TYPES_U64_CON } );

const TypePtr PrimitiveType::TYPE_F32  = create("f32", 4, vm::BinType::FLOAT, { CPP_TYPES_F32 }, { CPP_TYPES_F32_CON } );
const TypePtr PrimitiveType::TYPE_FLOAT = create("float", 8, vm::BinType::DOUBLE, { CPP_TYPES_F64 }, { CPP_TYPES_F64_CON } );

const TypePtr PrimitiveType::TYPE_BOOL = create("bool", 1, vm::BinType::UINT8, { CPP_TYPES_BOOL }, { CPP_TYPES_BOOL_CON } );

const TypePtr PrimitiveType::TYPE_VOID = create("void", 0, vm::BinType::UINT8, { CPP_TYPES_VOID }, { CPP_TYPES_VOID_CON } );

std::string PrimitiveType::to_string() const {
    return C_TYPE(name);
}

size_t PrimitiveType::get_size() const {
    return size;
}

Type::KIND PrimitiveType::get_kind() const {
	return PRIMITIVE;
}

bool PrimitiveType::is_cpp_type(const std::string &cpp_type) const {
	return std::find(cpp_types_synonymous.begin(), cpp_types_synonymous.end(), cpp_type) != cpp_types_synonymous.end();
}

bool PrimitiveType::is_convertable_to_cpp_type(const std::string &cpp_type) const {
	std::string cpp_type_normalize = cpp_type;

	if (cpp_type.starts_with("const")) {
		cpp_type_normalize = cpp_type.substr(std::strlen("const "));
	}

	return std::find(cpp_types_convertable.begin(), cpp_types_convertable.end(), cpp_type_normalize) != cpp_types_convertable.end();
}

std::string PrimitiveType::to_cpp_type_str() {
	return cpp_types_synonymous.front();
}

} // namespace cmp

static_assert(sizeof(int) == 4);
static_assert(sizeof(long) == 8);
