#include "swapchain.hpp"

#include "vulkan.hpp"

namespace auplib
{

auplib::Swapchain::Swapchain(VkSurfaceKHR surface, VkFormat image_format, VkExtent2D extent) {
	VkResult result;
	VkSurfaceCapabilitiesKHR surface_caps;
	result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vulkan_instance.phys_device, surface, &surface_caps);
	assert(result == VK_SUCCESS);

	VkSwapchainCreateInfoKHR swapchain_create_info{
		.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
		.pNext = nullptr,
		.flags = 0,
		.surface = surface,
		.minImageCount = surface_caps.minImageCount + 1,
		.imageFormat = image_format,
		.imageColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR,
		.imageExtent = extent,
		.imageArrayLayers = 1,
		.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
		.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
		.queueFamilyIndexCount = 0,
		.pQueueFamilyIndices = nullptr,
		.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
		.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
		.presentMode = VK_PRESENT_MODE_MAILBOX_KHR,
		.clipped = false,
		.oldSwapchain = VK_NULL_HANDLE,
	};

	result = vkCreateSwapchainKHR(vulkan_instance.device, &swapchain_create_info, nullptr, &swapchain);

	// get images
	std::vector<VkImage> images;

	result = vkGetSwapchainImagesKHR(vulkan_instance.device, swapchain, &image_count, nullptr);
	assert(result == VK_SUCCESS);
	
	images.resize(image_count);
	
	result = vkGetSwapchainImagesKHR(vulkan_instance.device, swapchain, &image_count, images.data());
	assert(result == VK_SUCCESS);

	// create render targets
	render_targets.resize(image_count);

	for (size_t i = 0; i < image_count; ++i) {
		render_targets[i] = RenderTarget{
			.image = images[i],
			.image_view = {},
			.format = image_format,
			.extent = extent
		};
		
		VkImageViewCreateInfo create_info{
			.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.image = render_targets[i].image,
			.viewType = VK_IMAGE_VIEW_TYPE_2D,
			.format = image_format,
			.components = {
				VK_COMPONENT_SWIZZLE_IDENTITY,
				VK_COMPONENT_SWIZZLE_IDENTITY,
				VK_COMPONENT_SWIZZLE_IDENTITY,
				VK_COMPONENT_SWIZZLE_IDENTITY
			},
			.subresourceRange = {
				.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
				.baseMipLevel = 0,
				.levelCount = 1,
				.baseArrayLayer = 0,
				.layerCount = 1
			}
		};

		result = vkCreateImageView(
			vulkan_instance.device,
			&create_info,
			nullptr,
			&render_targets[i].image_view
		);

		assert(result == VK_SUCCESS);
	}
}

} // namespace auplib