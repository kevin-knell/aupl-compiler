#pragma once

#include <vulkan/vulkan.h>
#include <assert.h>
#include <iostream>
#include <vector>
#include <array>
#include <vulkan/vulkan.h>

#include "string.hpp"
#include "window.hpp"

namespace auplib
{

struct VulkanInstance {
	VkInstance instance;
	VkPhysicalDevice phys_device;
	VkDevice device;

	uint32_t selected_queue_family_idx;
	VkQueue queue;

	VkDescriptorPool desc_pool;
	VkDescriptorSetLayout desc_set_layout_frame;
	VkDescriptorSetLayout desc_set_layout_object;
	VkDescriptorSetLayout desc_set_layout_sampler;

	VulkanInstance();
	COPY_DELETE(VulkanInstance)
	MOVE_DELETE(VulkanInstance)

	void create_instance();
	void create_device();
	uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

	static VulkanInstance* singleton();
};

} // namespace auplib


