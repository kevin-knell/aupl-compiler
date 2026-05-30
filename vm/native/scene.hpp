#pragma once

#include "node.hpp"

namespace auplib {

class Scene : public Object {
public:
	static void register_to_db(vm::ClassDB &db);
	Shared<Node> root;
};

} // namespace auplib
