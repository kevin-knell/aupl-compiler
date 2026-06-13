#pragma once

#include "vulkan.hpp"
#include "vec2i.hpp"

namespace auplib {

struct Image;

struct TextureRID_T {
private:
	struct VulkanData {
		VkImage image;
		VkImageView image_view;
		VkSampler sampler;
		VkDeviceMemory device_memory;
		VkImageLayout image_layout;
		VkDevice device;
		VkDescriptorSet sampler_descriptor_set;
	} vulkan_data{};

	void change_layout(VkCommandPool cmd_pool, VkQueue queue, VkImageLayout layout);
	void write_buffer_to_image(vec2i image_size, VkCommandPool cmd_pool, VkQueue queue, VkBuffer buffer);

public:
	void upload(Image* image, VkDevice device, VkCommandPool cmd_pool, VkQueue queue);
	
	VkSampler get_sampler() const { return vulkan_data.sampler; }
	VkImage get_vk_image() const { return vulkan_data.image; }
	VkImageView get_image_view() const { return vulkan_data.image_view; }
	VkImageLayout get_image_layout() const { return vulkan_data.image_layout; }
	VkDescriptorSet get_descriptor_set() const { return vulkan_data.sampler_descriptor_set; }
};

}