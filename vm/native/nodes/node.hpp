#pragma once

#include "object.hpp"
#include "shared.hpp"
#include "list.hpp"

namespace auplib
{

class Node : public Object {
	OBJECT_CLASS(Node, Object)
public:
	static void register_to_db(vm::ClassDB &db);

	static Shared<Node> compose(Shared<Node> node, List<Shared<Node>> nodes);

	Node() {}

	Shared<Node> parent;
	List<Shared<Node>> children;

	virtual void _on_tree_added() {};

	void add_child(Shared<Node> node);

	List<Shared<Node>> get_children() const { return children; }
};

} // namespace auplib
