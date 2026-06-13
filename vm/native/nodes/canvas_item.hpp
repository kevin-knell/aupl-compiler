#pragma once

#include <vulkan/vulkan.h>

#include "node.hpp"

#include "mat3.hpp"
#include "mat4.hpp"

#include "rid_forward_decl.hpp"

#include "ubo.hpp"

namespace auplib {

class CanvasItem : public Node {
	OBJECT_CLASS(CanvasItem, Node)
public:
	ObjectUBO* object_ubo;

	CanvasItem();
	CanvasItem(const CanvasItem&) = delete;
	CanvasItem& operator=(CanvasItem&) = delete;

private:
	CanvasItemRID canvas_item_rid;

public:
	static void register_to_db(vm::ClassDB &db);
	
	vec2 get_position() const {
		return {
			object_ubo->model.w.x,
			object_ubo->model.w.y
		};
	}

	void set_position(vec2 p) {
		object_ubo->model.w.x = p.x;
		object_ubo->model.w.y = p.y;
	}
	
	void set_size(vec2 s) {
		object_ubo->size.x = s.x;
		object_ubo->size.y = s.y;
	};

	vec2 get_size() const {
		return vec2(object_ubo->size.x, object_ubo->size.y);
	};

	void _on_tree_added() override;

	CanvasItemRID get_canvas_item_rid() const {
		return canvas_item_rid;
	}
};

} // namespace auplib
