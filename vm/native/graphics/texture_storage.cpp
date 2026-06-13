#include "texture_storage.hpp"

#include "texture_rid.hpp"
#include "image.hpp"

namespace auplib {

TextureStorage::TextureStorage() {
	VkResult result;

	// command pool
	VkCommandPoolCreateInfo pool_info{
		.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
		.pNext = nullptr,
		.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
		.queueFamilyIndex = VulkanInstance::singleton()->selected_queue_family_idx
	};

	result = vkCreateCommandPool(VulkanInstance::singleton()->device, &pool_info, nullptr, &command_pool);
	assert(result == VK_SUCCESS);

	// command buffers
	VkCommandBufferAllocateInfo alloc_info{
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		.pNext = nullptr,
		.commandPool = command_pool,
		.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
		.commandBufferCount = 1
	};

	result = vkAllocateCommandBuffers(VulkanInstance::singleton()->device, &alloc_info, &command_buffer);
	assert(result == VK_SUCCESS);
}

TextureStorage *TextureStorage::singleton() {
	static TextureStorage _singleton;
	return &_singleton;
}

TextureRID TextureStorage::create_texture(Image* image) {
	assert(!!image);
	
	TextureRID_T* texture = new TextureRID_T();

	texture->upload(image, VulkanInstance::singleton()->device, command_pool, VulkanInstance::singleton()->queue);
	
	TextureRID result = texture;
	return result;
}

} // namespace auplib
