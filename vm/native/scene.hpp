#pragma once

#include "node.hpp"
#include "macros.hpp"

namespace auplib {

class Scene : public Object {
public:
	static void register_to_db(vm::ClassDB &db);
	Shared<Node> root;

	SETGET(root)
};

} // namespace auplib
