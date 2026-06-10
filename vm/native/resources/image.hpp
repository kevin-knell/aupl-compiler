#pragma once

#include <stb/stb_image.h>

#include "resource.hpp"
#include "vulkan.hpp"
#include "vec2i.hpp"
#include "color.hpp"

namespace auplib {

class Image final : public Resource {
private:
	stbi_uc* raw = nullptr;
	vec2i size = vec2i();
	int channels_in_file = 0;

	struct VulkanData {
		VkImage image = VK_NULL_HANDLE;
		VkImageView image_view = VK_NULL_HANDLE;
		VkSampler sampler = VK_NULL_HANDLE;
		VkDeviceMemory device_memory = VK_NULL_HANDLE;
		VkImageLayout image_layout = VK_IMAGE_LAYOUT_UNDEFINED;
		VkDevice device = VK_NULL_HANDLE;
	} vulkan_data;

public:
	static Shared<Image> load_from_file(String path);
	static Shared<Image> create_fill(vec2i size, Color fill_color);

	Image(RcKey rc_key, vec2i size, Color fill_color);
	Image(RcKey rc_key, String path);

	Image(const Image& other) = default;
	Image& operator=(const Image& other) = default;
	
	Image(Image&& other) = default;
	Image& operator=(Image&& other) = default;

	~Image() = default;

	vec2i get_size() const { return size; }
	size_t get_data_size() const;

	void upload(VkDevice device, VkCommandPool cmd_pool, VkQueue queue);
	void change_layout(VkCommandPool cmd_pool, VkQueue queue, VkImageLayout layout);
	void write_buffer_to_image(VkCommandPool cmd_pool, VkQueue queue, VkBuffer buffer);

	VkSampler get_sampler() const { return vulkan_data.sampler; }
	VkImage get_vk_image() const { return vulkan_data.image; }
	VkImageView get_image_view() const { return vulkan_data.image_view; }
	VkImageLayout get_image_layout() const { return vulkan_data.image_layout; }

	void destroy();
};

} // namespace auplib
