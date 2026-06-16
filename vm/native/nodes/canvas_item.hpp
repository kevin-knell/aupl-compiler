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
	vec2 size;

	CanvasItem();
	CanvasItem(const CanvasItem&) = delete;
	CanvasItem& operator=(CanvasItem&) = delete;

private:
	CanvasItemRID canvas_item_rid;

public:
	static void register_to_db(vm::ClassDB &db);
	
	vec2 get_position() const {
		return {
			item_data->model.w.x,
			item_data->model.w.y
		};
	}

	void set_position(vec2 p) {
		item_data->model.w.x = p.x;
		item_data->model.w.y = p.y;
	}
	
	void set_rotation(float angle) {
		mat2 rot_mat = mat2::from_angle(angle);

		item_data->model.x.x = rot_mat.x.x;
		item_data->model.x.y = rot_mat.x.y;
		item_data->model.y.x = rot_mat.y.x;
		item_data->model.y.y = rot_mat.y.y;
	}

	float get_rotation() const {
		return vec2(
			item_data->model.x.x,
			item_data->model.y.x
		).angle();
	}
	
	void set_scale(float scale) {
		mat2 scale_mat = mat2(
			{ item_data->model.x.x, item_data->model.x.y },
			{ item_data->model.y.x, item_data->model.y.y }
		);

		float det = scale_mat.det();

		if (det == 0.0) scale_mat = mat2( vec2::EX, vec2::EY );

		scale_mat = scale_mat / scale_mat.det() * scale;

		item_data->model.x.x = scale_mat.x.x;
		item_data->model.x.y = scale_mat.x.y;
		item_data->model.y.x = scale_mat.y.x;
		item_data->model.y.y = scale_mat.y.y;
	}

	float get_scale() const {
		mat2 scale_mat = mat2(
			{ item_data->model.x.x, item_data->model.x.y },
			{ item_data->model.y.x, item_data->model.y.y }
		);

		return scale_mat.det();
	}

	void _on_tree_added() override;

	CanvasItemRID get_canvas_item_rid() const {
		return canvas_item_rid;
	}
};

} // namespace auplib
