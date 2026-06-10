#include "image.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

namespace auplib {

// constructors
Image::Image(RcKey rc_key, String path) : Resource(rc_key, path) {
	raw = stbi_load(path, &size.x, &size.y, &channels_in_file, STBI_rgb_alpha);
	assert(!!raw);
	res_data.path = path;
}

Image::Image(RcKey rc_key, vec2i size, Color fill_color) : Resource(rc_key) {
	this->size = size;
	size_t pixel_amount = size.area();
	
	channels_in_file = 4;
	raw = new stbi_uc[get_data_size()];

	uint32_t* raw_u32 = reinterpret_cast<uint32_t*>(raw);

	uint32_t color_u32 = fill_color.get_r8g8b8a8_unorm();
	
	std::fill_n(raw_u32, pixel_amount, color_u32);
}

// create methods
Shared<Image> Image::load_from_file(String path) {
	return Shared<Image>::make(path);
}

Shared<Image> Image::create_fill(vec2i size, Color fill_color) {
	return Shared<Image>::make(size, fill_color);
}

// 
size_t Image::get_data_size() const {
	return size.area() * channels_in_file;
}

void Image::upload(VkDevice device, VkCommandPool cmd_pool, VkQueue queue) {
	assert(!!raw);

	vulkan_data.device = device;

	VkDeviceSize data_size = get_data_size();

	VkBuffer staging_buffer;
	VkDeviceMemory staging_buffer_memory;

	VkBufferCreateInfo buffer_create_info = {
	.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
	.pNext = nullptr,
	.flags = 0,
	.size = data_size,
	.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
	.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
	.queueFamilyIndexCount = 0,
	.pQueueFamilyIndices = 0,
	};

	VkResult result = vkCreateBuffer(vulkan_data.device, &buffer_create_info, nullptr, &staging_buffer);
	assert(result == VK_SUCCESS);

	VkMemoryRequirements mem_req;
	vkGetBufferMemoryRequirements(vulkan_data.device, staging_buffer, &mem_req);

	VkMemoryAllocateInfo mem_alloc_info = {
		.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
		.pNext = nullptr,
		.allocationSize = mem_req.size,
		.memoryTypeIndex = vulkan_instance.findMemoryType(
			mem_req.memoryTypeBits,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
			VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		)
	};

	result = vkAllocateMemory(vulkan_data.device, &mem_alloc_info, nullptr, &staging_buffer_memory);
	assert(result == VK_SUCCESS);

	result = vkBindBufferMemory(vulkan_data.device, staging_buffer, staging_buffer_memory, 0);
	assert(result == VK_SUCCESS);

	void* raw_data;
	vkMapMemory(vulkan_data.device, staging_buffer_memory, 0, buffer_create_info.size, 0, &raw_data);
	std::memcpy(raw_data, raw, mem_req.size);

	VkImageCreateInfo image_create_info {
		.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.imageType = VK_IMAGE_TYPE_2D,
		.format = VK_FORMAT_R8G8B8A8_UNORM,
		.extent = { static_cast<uint32_t>(size.x), static_cast<uint32_t>(size.y), 1 },
		.mipLevels = 1,
		.arrayLayers = 1,
		.samples = VK_SAMPLE_COUNT_1_BIT,
		.tiling = VK_IMAGE_TILING_OPTIMAL,
		.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
		.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
		.queueFamilyIndexCount = 0,
		.pQueueFamilyIndices = nullptr,
		.initialLayout = vulkan_data.image_layout,
	};

	result = vkCreateImage(vulkan_data.device, &image_create_info, nullptr, &vulkan_data.image);
	assert(result == VK_SUCCESS);

	VkMemoryRequirements img_mem_req;
	vkGetImageMemoryRequirements(vulkan_data.device, vulkan_data.image, &img_mem_req);

	VkMemoryAllocateInfo img_mem_alloc_info{
		.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
		.pNext = nullptr,
		.allocationSize = img_mem_req.size,
		.memoryTypeIndex = vulkan_instance.findMemoryType(img_mem_req.memoryTypeBits, 
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
			VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)
	};

	result = vkAllocateMemory(vulkan_data.device, &img_mem_alloc_info, nullptr, &vulkan_data.device_memory);
	assert(result == VK_SUCCESS);

	result = vkBindImageMemory(vulkan_data.device, vulkan_data.image, vulkan_data.device_memory, 0);
	assert(result == VK_SUCCESS);

	change_layout(cmd_pool, queue, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

	write_buffer_to_image(cmd_pool, queue, staging_buffer);

	change_layout(cmd_pool, queue, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	vkDestroyBuffer(vulkan_data.device, staging_buffer, nullptr);
	vkFreeMemory(vulkan_instance.device, staging_buffer_memory, nullptr);

	VkImageViewCreateInfo img_view_create_info{
		.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.image = vulkan_data.image,
		.viewType = VK_IMAGE_VIEW_TYPE_2D,
		.format = VK_FORMAT_R8G8B8A8_UNORM,
		.components = {
			.r = VK_COMPONENT_SWIZZLE_R,
			.g = VK_COMPONENT_SWIZZLE_G,
			.b = VK_COMPONENT_SWIZZLE_B,
			.a = VK_COMPONENT_SWIZZLE_A,
		},
		.subresourceRange = {
			.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
			.baseMipLevel = 0,
			.levelCount = 1,
			.baseArrayLayer = 0,
			.layerCount = 1,
		},
	};

	result = vkCreateImageView(vulkan_data.device, &img_view_create_info, nullptr, &vulkan_data.image_view);
	assert(result == VK_SUCCESS);

	VkSamplerCreateInfo sampler_create_info{
		.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.magFilter = VK_FILTER_LINEAR,
		.minFilter = VK_FILTER_LINEAR,
		.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
		.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT,
		.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT,
		.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT,
		.mipLodBias = 0.0f,
		.anisotropyEnable = VK_TRUE,
		.maxAnisotropy = 16,
		.compareEnable = VK_FALSE,
		.compareOp = VK_COMPARE_OP_ALWAYS,
		.minLod = 0.0f,
		.maxLod = 0.0f,
		.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK,
		.unnormalizedCoordinates = VK_FALSE,
	};

	result = vkCreateSampler(vulkan_data.device, &sampler_create_info, nullptr, &vulkan_data.sampler);
}

void Image::change_layout(VkCommandPool cmd_pool, VkQueue queue, VkImageLayout layout) {
	VkCommandBuffer cmd_buffer;

	VkCommandBufferAllocateInfo alloc_info{
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		.pNext = nullptr,
		.commandPool = cmd_pool,
		.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
		.commandBufferCount = 1
	};

	VkResult result = vkAllocateCommandBuffers(vulkan_data.device, &alloc_info, &cmd_buffer);
	assert(result == VK_SUCCESS);

	VkCommandBufferBeginInfo cmd_buffer_begin_info{
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		.pNext = nullptr,
		.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
		.pInheritanceInfo = nullptr,
	};

	result = vkBeginCommandBuffer(cmd_buffer, &cmd_buffer_begin_info);
	assert(result == VK_SUCCESS);
	
	VkAccessFlags src_access_mask;
	VkAccessFlags dst_access_mask;
	VkPipelineStageFlags src_stage;
	VkPipelineStageFlags dst_stage;

	if (vulkan_data.image_layout == VK_IMAGE_LAYOUT_UNDEFINED
			&& layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
		src_access_mask = VK_ACCESS_NONE;
		dst_access_mask = VK_ACCESS_TRANSFER_WRITE_BIT;
		src_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		dst_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	} else if (vulkan_data.image_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
			&& layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
		src_access_mask = VK_ACCESS_TRANSFER_WRITE_BIT;
		dst_access_mask = VK_ACCESS_SHADER_READ_BIT;
		src_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		dst_stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	} else {
		std::cerr << "invalid layout transition" << std::endl;
		abort();
	}

	// barrier
	VkImageMemoryBarrier barrier{
		.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
		.pNext = nullptr,
		.srcAccessMask = src_access_mask,
		.dstAccessMask = dst_access_mask,
		.oldLayout = vulkan_data.image_layout,
		.newLayout = layout,
		.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.image = vulkan_data.image,
		.subresourceRange = {
			.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
			.baseMipLevel = 0,
			.levelCount = 1,
			.baseArrayLayer = 0,
			.layerCount = 1
		}
	};

	vkCmdPipelineBarrier(
		cmd_buffer,
		src_stage,
		dst_stage,
		0,
		0, nullptr,
		0, nullptr,
		1, &barrier
	);
	
	vkEndCommandBuffer(cmd_buffer);

		VkSubmitInfo submit_info{
		.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
		.pNext = nullptr,
		.waitSemaphoreCount = 0,
		.pWaitSemaphores = nullptr,
		.pWaitDstStageMask = nullptr,
		.commandBufferCount = 1,
		.pCommandBuffers = &cmd_buffer,
		.signalSemaphoreCount = 0,
		.pSignalSemaphores = nullptr,
	};

	result = vkQueueSubmit(queue, 1, &submit_info, VK_NULL_HANDLE);
	assert(result == VK_SUCCESS);

	result = vkQueueWaitIdle(queue);
	assert(result == VK_SUCCESS);

	vkFreeCommandBuffers(vulkan_data.device, cmd_pool, 1, &cmd_buffer);

	vulkan_data.image_layout = layout;
}

void Image::write_buffer_to_image(VkCommandPool cmd_pool, VkQueue queue, VkBuffer buffer) {
	VkCommandBuffer cmd_buffer;

	VkCommandBufferAllocateInfo alloc_info{
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		.pNext = nullptr,
		.commandPool = cmd_pool,
		.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
		.commandBufferCount = 1
	};

	VkResult result = vkAllocateCommandBuffers(vulkan_data.device, &alloc_info, &cmd_buffer);
	assert(result == VK_SUCCESS);

	VkCommandBufferBeginInfo cmd_buffer_begin_info{
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		.pNext = nullptr,
		.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
		.pInheritanceInfo = nullptr,
	};

	result = vkBeginCommandBuffer(cmd_buffer, &cmd_buffer_begin_info);
	assert(result == VK_SUCCESS);


	VkBufferImageCopy img_buffer_copy{
		.bufferOffset = 0,
		.bufferRowLength = 0,
		.bufferImageHeight = 0,
		.imageSubresource = {
			.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
			.mipLevel = 0,
			.baseArrayLayer = 0,
			.layerCount = 1,
		},
		.imageOffset = { 0, 0, 0 },
		.imageExtent = { static_cast<uint32_t>(size.x), static_cast<uint32_t>(size.y), 1 },
	};

	vkCmdCopyBufferToImage(cmd_buffer, buffer, vulkan_data.image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &img_buffer_copy);

	vkEndCommandBuffer(cmd_buffer);

		VkSubmitInfo submit_info{
		.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
		.pNext = nullptr,
		.waitSemaphoreCount = 0,
		.pWaitSemaphores = nullptr,
		.pWaitDstStageMask = nullptr,
		.commandBufferCount = 1,
		.pCommandBuffers = &cmd_buffer,
		.signalSemaphoreCount = 0,
		.pSignalSemaphores = nullptr,
	};

	result = vkQueueSubmit(queue, 1, &submit_info, VK_NULL_HANDLE);
	assert(result == VK_SUCCESS);

	result = vkQueueWaitIdle(queue);
	assert(result == VK_SUCCESS);

	vkFreeCommandBuffers(vulkan_data.device, cmd_pool, 1, &cmd_buffer);
}

void Image::destroy() {
	stbi_image_free(raw);

	vkDestroySampler(vulkan_data.device, vulkan_data.sampler, nullptr);
	vkDestroyImageView(vulkan_data.device, vulkan_data.image_view, nullptr);
	vkDestroyImage(vulkan_data.device, vulkan_data.image, nullptr);
	vkFreeMemory(vulkan_instance.device, vulkan_data.device_memory, nullptr);
}

}