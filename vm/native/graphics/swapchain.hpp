#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include "render_target.hpp"

namespace auplib
{

class Swapchain {
public:
	VkSurfaceKHR surface;
	VkFormat image_format;
	VkExtent2D extent;
	
	VkSwapchainKHR vk_swapchain = VK_NULL_HANDLE;
	std::vector<RenderTarget> render_targets;
	uint32_t image_count;

private:
	void create();
	void create_vk_swapchain();
	std::vector<VkImage> get_vk_images();
	void create_render_targets();

public:
	Swapchain() = default;
	Swapchain(VkSurfaceKHR surface, VkFormat image_format, VkExtent2D extent);

	void recreate(VkExtent2D new_extent);
};

} // namespace auplib
