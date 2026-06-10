#pragma once

#include <vulkan/vulkan.h>

#include "node.hpp"

#include "mat3.hpp"
#include "mat4.hpp"

#include "image.hpp"

namespace auplib {

class CanvasItem : public Node {
public:
	struct ObjectUniformData {
		mat4 model;
		mat4 model_view;
		mat4 model_view_projection;

		mat3 normal_matrix;
	} object_data;

	void* object_uniform_mapped;
	VkBuffer object_uniform_buffer;
	VkDeviceMemory object_uniform_memory;
	VkDescriptorSet object_descriptor_set;
	VkDescriptorSet sampler_descriptor_set;
	Shared<Image> image = Shared<Image>::make(vec2i(1, 1), Color(1.0));

	CanvasItem() = default;
	CanvasItem(const CanvasItem&) = delete;
	CanvasItem& operator=(CanvasItem&) = delete;
	
	vec2 get_position() const {
		return {
			object_data.model.w.x,
			object_data.model.w.y
		};
	}

	void set_position(vec2 p) {
		object_data.model.w.x = p.x;
		object_data.model.w.y = p.y;
	}
	
	vec2 get_size() const;

	void init(VkCommandPool cmd_pool);
};

} // namespace auplib
