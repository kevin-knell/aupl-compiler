#include "primitive_type.hpp"

#include "color.hpp"

namespace cmp
{

TypePtr create(std::string n, int s) {
    PrimitiveType result = PrimitiveType(n, s);
    return std::make_shared<PrimitiveType>(result);
}

const TypePtr PrimitiveType::TYPE_INT = create("int", 8);
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
