#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <assert.h>
#include <iostream>
#include <vector>
#include <array>
#include <vulkan/vulkan.h>

#include "string.hpp"
#include "window.hpp"

namespace auplib
{

extern VkInstance vk_instance;
extern VkPhysicalDevice vk_phys_device;
extern VkDevice vk_device;
extern VkQueue vk_queue;

void init_vulkan();
void create_swapchain(Window& window);
VkShaderModule load_shader(String path);


} // namespace auplib


