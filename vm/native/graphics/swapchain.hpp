#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include "render_target.hpp"

namespace auplib
{

class Swapchain {
public:
	VkSwapchainKHR swapchain;
	std::vector<RenderTarget> render_targets;
	uint32_t image_count;

	Swapchain() = default;
	Swapchain(VkSurfaceKHR surface, VkFormat image_format, VkExtent2D extent);
};

} // namespace auplib
