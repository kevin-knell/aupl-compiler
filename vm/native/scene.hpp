#pragma once

#include "node.hpp"

namespace auplib {

class Scene : public Object {
public:
	static void register_to_db(vm::ClassDB &db);
	Shared<Node> root;

	void set_root(Shared<Node> r) { root = r; }
	Shared<Node> get_root() const { return root; }
};

} // namespace auplib
