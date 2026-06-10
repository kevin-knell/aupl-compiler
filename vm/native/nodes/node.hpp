#pragma once

#include "object.hpp"
#include "shared.hpp"
#include "list.hpp"

namespace auplib
{

class Node : public Object {
public:
	static void register_to_db(vm::ClassDB &db);

	Shared<Node> parent;
	List<Shared<Node>> children;

	static Shared<Node> compose(Shared<Node> node, List<Shared<Node>> nodes);

	void add_child(Shared<Node> node);

	List<Shared<Node>> get_children() const { return children; }
};

} // namespace auplib
