#pragma once
#include "object.hpp"
#include "native.hpp"

namespace vm
{
    class ClassDB;
} // namespace vm


class ClassBind : Object {
public:
    static void register_to_db(vm::ClassDB& db);
};