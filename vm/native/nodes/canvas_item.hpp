#pragma once

#include <vulkan/vulkan.h>

#include "node.hpp"

#include "mat3.hpp"
#include "mat4.hpp"

#include "rid_forward_decl.hpp"

#include "vertex.hpp"

namespace auplib {

class CanvasItem : public Node {
	OBJECT_CLASS(CanvasItem, Node)
public:
	InstanceDataBase* instance_data;

	CanvasItem();
	CanvasItem(const CanvasItem&) = delete;
	CanvasItem& operator=(CanvasItem&) = delete;

private:
	CanvasItemRID canvas_item_rid;

public:
	static void register_to_db(vm::ClassDB &db);
	
	vec2 get_position() const {
		return {
			instance_data->model.w.x,
			instance_data->model.w.y
		};
	}

	void set_position(vec2 p) {
		instance_data->model.w.x = p.x;
		instance_data->model.w.y = p.y;
	}
	
	void set_size(vec2 s) {
		instance_data->size.x = s.x;
		instance_data->size.y = s.y;
	};

	vec2 get_size() const {
		return vec2(instance_data->size.x, instance_data->size.y);
	};

	void _on_tree_added() override;

	CanvasItemRID get_canvas_item_rid() const {
		return canvas_item_rid;
	}
};

} // namespace auplib
