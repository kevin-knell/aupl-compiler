#include "symbol_builder.hpp"
#include "primitive_type.hpp"
#include "class_type.hpp"
#include "tuple_type.hpp"

namespace cmp {

TypePtr SymbolBuilder::parse_type(ParserInfo& parser_info) {
    if (auto result = parse_tuple_type(parser_info)) return result;

    if (auto result = parse_primitive_type(parser_info)) return result;
    if (auto result = parse_class_type(parser_info)) return result;
    if (auto result = parse_tuple_type(parser_info)) return result;

    return nullptr;
}

TypePtr SymbolBuilder::parse_primitive_type(ParserInfo& parser_info) {
    (void)parser_info;

    if (expect("i8"))    return next(), PrimitiveType::TYPE_I8;
    if (expect("i16"))   return next(), PrimitiveType::TYPE_I16;
    if (expect("i32"))   return next(), PrimitiveType::TYPE_I32;
    if (expect("int"))   return next(), PrimitiveType::TYPE_INT;

    if (expect("u8"))    return next(), PrimitiveType::TYPE_U8;
    if (expect("u16"))   return next(), PrimitiveType::TYPE_U16;
    if (expect("u32"))   return next(), PrimitiveType::TYPE_U32;
    if (expect("u64"))   return next(), PrimitiveType::TYPE_U64;

    if (expect("f32"))   return next(), PrimitiveType::TYPE_F32;
    if (expect("float")) return next(), PrimitiveType::TYPE_FLOAT;

    if (expect("bool"))  return next(), PrimitiveType::TYPE_BOOL;
    if (expect("void"))  return next(), PrimitiveType::TYPE_VOID;

    else return nullptr;
}

TypePtr SymbolBuilder::parse_class_type(ParserInfo& parser_info) {
    (void)parser_info;

    if (match(TokenType::IDENTIFIER)) {
        //return std::make_shared<ClassType>(next().value);
        // TODO: MyClass<T>
    }

    return nullptr;
}

TypePtr SymbolBuilder::parse_tuple_type(ParserInfo& parser_info) {
    (void)parser_info;
    
    size_t idx = index;

    if (!expect("(")) {
        return nullptr;
    }
    next(); // consume (

    std::vector<TypePtr> types;

    while (!expect(")")) {
        if (expect(",")) {
            if (!types.empty()) next(); // consume ,
            else return nullptr;
        }

        TypePtr t = parse_type(parser_info);
        if (!t) {
            index = idx;
            return nullptr;
        }
        types.push_back(t);
    }
    next(); // consume )

    return std::make_shared<TupleType>(types);
}

}