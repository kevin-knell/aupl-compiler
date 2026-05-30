#pragma once
#include <vulkan/vulkan.h>


namespace auplib
{

class RenderTarget {
public:
	VkImage image;
	VkImageView image_view;
	VkFormat format;
	VkExtent2D extent;
};

} // namespace auplib
