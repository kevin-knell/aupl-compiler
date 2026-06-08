#include "node.hpp"

namespace auplib {

void Node::register_to_db(vm::ClassDB &db) {
	const int16_t ID = REGISTER_CLASS(Node);
	
	REGISTER_CONSTRUCTOR(ID, Node());

	REGISTER_METHOD(ID, Node, add_child, void (Node::*)(Shared<Node> node));

	REGISTER_VARIABLE(ID, List<Shared<Node>>, children);
	REGISTER_VARIABLE(ID, Shared<Node>, root);
}

Shared<Node> Node::compose(Shared<Node> node, List<Shared<Node>> nodes) {
	for (size_t i = 0; i < nodes.size(); ++i) {
		node->add_child(nodes[i]);
	}
	
	return node;
}

void Node::add_child(Shared<Node> node)
{
	children.push(node);
}
}