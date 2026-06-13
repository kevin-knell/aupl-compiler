#pragma once

#include <vulkan/vulkan.h>
#include "render_command.hpp"
#include "ubo.hpp"

namespace auplib {

using CanvasItemRID = CanvasItemRID_T*;

struct CanvasItemRID_T {
	CanvasItemRID_T* parent = nullptr;
	CanvasItemRID_T* next = nullptr;
	RenderCommand* commands;

	ObjectUBO object_ubo;
	void* object_uniform_mapped;
	VkBuffer object_uniform_buffer;
	VkDeviceMemory object_uniform_memory;
	VkDescriptorSet object_descriptor_set;

	void init();
};

}