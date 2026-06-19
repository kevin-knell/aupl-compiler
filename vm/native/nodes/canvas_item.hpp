#pragma once

#include <vulkan/vulkan.h>

#include "node.hpp"

#include "mat2.hpp"
#include "mat3.hpp"
#include "mat4.hpp"

#include "rid_forward_decl.hpp"

#include "vertex.hpp"

namespace auplib {

class CanvasItem : public Node {
	OBJECT_CLASS(CanvasItem, Node)
public:
	ItemData* item_data;
	vec2 position = vec2(0, 0);
	float rotation = 0;
	vec2 scale = vec2(1.0, 1.0);

	CanvasItem();
	CanvasItem(const CanvasItem&) = delete;
	CanvasItem& operator=(CanvasItem&) = delete;

private:
	CanvasItemRID canvas_item_rid;

public:
	static void register_to_db(vm::ClassDB &db);
	
	vec2 get_position() const {
		return position;
	}

	void set_position(vec2 p) {
		position = p;
		item_data->model.w.x = position.x;
		item_data->model.w.y = position.y;
	}
	
	void set_rotation(float angle) {
		rotation = angle;
		update_model();
	}

	float get_rotation() const {
		return rotation;
	}
	
	void set_scale(vec2 _scale) {
		scale = _scale;
		update_model();
	}

	vec2 get_scale() const {
		return scale;
	}

	void _on_tree_added() override;

	CanvasItemRID get_canvas_item_rid() const {
		return canvas_item_rid;
	}

private:
	void update_model() {
		mat2 rot_mat = mat2::from_angle(rotation);

		mat2 scale_mat = mat2(
			{ scale.x, 0.0 },
			{ 0.0, scale.y }
		);

		mat2 xform = scale_mat * rot_mat;

		item_data->model.x.x = xform.x.x;
		item_data->model.x.y = xform.x.y;
		item_data->model.y.x = xform.y.x;
		item_data->model.y.y = xform.y.y;
	}
};

} // namespace auplib
