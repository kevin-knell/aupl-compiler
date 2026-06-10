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
	Image image = Image(vec2i(1, 1), Color(1.0, 1.0, 1.0, 1.0));

	CanvasItem();
	CanvasItem(const CanvasItem&) = delete;
	CanvasItem& operator=(CanvasItem&) = delete;
	void draw();

	void init(VkCommandPool cmd_pool);
};

} // namespace auplib
