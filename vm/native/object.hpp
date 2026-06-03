#pragma once

#include "native.hpp"

namespace auplib {

class Object {
public:
	static void register_to_db(vm::ClassDB& db);
    virtual ~Object() = default;
	//virtual String to_string() { return this; };
};

}