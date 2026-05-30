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

	VulkanInstance();

	void create_instance();
	void create_device();
	uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
};

struct PushConstant {
	float position[2];
	float size[2];
	Color color;
};

extern VulkanInstance vulkan_instance;


} // namespace auplib


