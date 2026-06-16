#include "canvas_item.hpp"

#include "vulkan.hpp"

#include "canvas_item_rid.hpp"

namespace auplib {
CanvasItem::CanvasItem() {
	canvas_item_rid = new CanvasItemRID_T();
	canvas_item_rid->init();
	item_data = &canvas_item_rid->item_data;
}

void CanvasItem::register_to_db(vm::ClassDB &db) {
	const uint16_t ID = REGISTER_OBJECT_CLASS(CanvasItem, Node);

	REGISTER_SETGET(ID, CanvasItem, vec2, position);
	REGISTER_SETGET(ID, CanvasItem, float, rotation);
	REGISTER_SETGET(ID, CanvasItem, float, scale);
}

void CanvasItem::_on_tree_added() {
	if (!parent) return;

	List<Shared<Node>> siblings = parent->get_children();

	if (siblings.empty()) return;

	for (size_t i = 0; i < siblings.size(); ++i) {
		Shared<Node> sibling = siblings[siblings.size() - 1 - i];

		assert(!!sibling);

		if (sibling.get() == this) continue;

		CanvasItem* sibling_ci = cast_to<CanvasItem>(sibling.get());

		if (!sibling_ci) {
			continue;
		}

		CanvasItemRID sibling_rid = sibling_ci->canvas_item_rid;
		CanvasItemRID_T* item = sibling_rid;
		item->next = canvas_item_rid;
		return;
	}

	CanvasItem* parent_ci = cast_to<CanvasItem>(parent.get());

	if (parent_ci) {
		CanvasItemRID parent_rid = parent_ci->canvas_item_rid;
		CanvasItemRID_T* item = parent_rid;
		assert(!item->next);
		item->next = canvas_item_rid;
	}
}

}