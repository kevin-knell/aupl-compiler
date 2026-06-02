#pragma once

#include <vulkan/vulkan.h>

#include "node.hpp"
#include "mat3.hpp"
#include "mat4.hpp"

namespace auplib
{

class CanvasItem : public Node {
public:
	struct ObjectUniformData {
		mat4 model;
		mat4 model_view;
		mat4 model_view_projection;

		mat3 normal_matrix;
	};

	ObjectUniformData object_data;
	void* object_uniform_mapped;
	VkBuffer object_uniform_buffer;
	VkDeviceMemory object_uniform_memory;
	VkDescriptorSet object_descriptor_set;

	CanvasItem();
	CanvasItem(const CanvasItem&) = delete;
	CanvasItem& operator=(CanvasItem&) = delete;
	void draw();
};

} // namespace auplib
