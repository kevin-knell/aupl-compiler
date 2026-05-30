#include "node.hpp"

namespace auplib {

void Node::register_to_db(vm::ClassDB &db) {
	const int16_t ID = REGISTER_CLASS(Node);
	
	REGISTER_CONSTRUCTOR(ID, Node());

	REGISTER_METHOD(ID, Node, add_child, void (Node::*)(Shared<Node> node));

	REGISTER_VARIABLE(ID, List<Shared<Node>>, children);
	REGISTER_VARIABLE(ID, Shared<Node>, root);
}

void Node::add_child(Shared<Node> node) {
	children.push(node);
}

}