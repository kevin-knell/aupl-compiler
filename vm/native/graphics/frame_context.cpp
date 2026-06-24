#include "frame_context.hpp"
#include <assert.h>

#include "vulkan.hpp"

namespace auplib {

FrameContext::FrameContext() {
	VkResult result;

	// frame uniform buffer
	VkDeviceSize size = sizeof(FrameUniformData);

	VkBufferCreateInfo buffer_create_info{
		.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.size = size,
		.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
		.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
		.queueFamilyIndexCount = 0,
		.pQueueFamilyIndices = nullptr
	};

	result = vkCreateBuffer(VulkanInstance::singleton()->device, &buffer_create_info, nullptr, &frame_uniform_buffer);
	assert(result == VK_SUCCESS);

	// frame uniform memory
	VkMemoryRequirements mem_req;
	vkGetBufferMemoryRequirements(VulkanInstance::singleton()->device, frame_uniform_buffer, &mem_req);

	VkMemoryAllocateInfo alloc_info{
		.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
		.pNext = nullptr,
		.allocationSize = mem_req.size,
		.memoryTypeIndex = VulkanInstance::singleton()->find_memory_type(
			mem_req.memoryTypeBits,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
			VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		)
	};

	result = vkAllocateMemory(VulkanInstance::singleton()->device, &alloc_info, nullptr, &frame_uniform_memory);
	assert(result == VK_SUCCESS);

	result = vkBindBufferMemory(VulkanInstance::singleton()->device, frame_uniform_buffer, frame_uniform_memory, 0);
	assert(result == VK_SUCCESS);

	// semaphores
	VkSemaphoreCreateInfo semaphore_info{
		.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
		.pNext = nullptr,
    	.flags = 0
	};

	result = vkCreateSemaphore(
		VulkanInstance::singleton()->device,
		&semaphore_info,
		nullptr,
		&image_available_semaphore
	);
	assert(result == VK_SUCCESS);

	result = vkCreateSemaphore(
		VulkanInstance::singleton()->device,
		&semaphore_info,
		nullptr,
		&render_finished_semaphore
	);
	assert(result == VK_SUCCESS);

	// fence
	VkFenceCreateInfo fence_info{
    	.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
		.pNext = nullptr,
    	.flags = VK_FENCE_CREATE_SIGNALED_BIT
	};

	result = vkCreateFence(VulkanInstance::singleton()->device, &fence_info, nullptr, &in_flight_fence);
	assert(result == VK_SUCCESS);
}

void FrameContext::record_begin(const RenderTarget& render_target) {
	VkResult result;

	vkResetCommandBuffer(command_buffer, 0);

	// command buffer begin
	VkCommandBufferBeginInfo cmd_buffer_begin_info{
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		.pNext = nullptr,
		.flags = 0,
		.pInheritanceInfo = nullptr,
	};

	result = vkBeginCommandBuffer(command_buffer, &cmd_buffer_begin_info);
	assert(result == VK_SUCCESS);

	// barrier
	VkImageMemoryBarrier barrier{
		.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
		.pNext = nullptr,
		.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT,
		.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
		.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
		.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
		.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.image = render_target.image,
		.subresourceRange = {
			.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
			.baseMipLevel = 0,
			.levelCount = 1,
			.baseArrayLayer = 0,
			.layerCount = 1
		}
	};

	vkCmdPipelineBarrier(
		command_buffer,
		VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		0,
		0, nullptr,
		0, nullptr,
		1, &barrier
	);

	// begin rendering
	VkClearValue clear_value{
		.color = { 0.0f, 0.0f, 0.0f, 1.0f }
	};

	VkRenderingAttachmentInfo color_attachment{
		.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
		.pNext = nullptr,
		.imageView = render_target.image_view,
		.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
		.resolveMode = VK_RESOLVE_MODE_NONE,
		.resolveImageView = VK_NULL_HANDLE,
		.resolveImageLayout = VK_IMAGE_LAYOUT_UNDEFINED,
		.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
		.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
		.clearValue = clear_value,
	};

	VkRenderingInfo rendering_info {
		.sType = VK_STRUCTURE_TYPE_RENDERING_INFO,
		.pNext = nullptr,
		.flags = 0,
		.renderArea = { .offset = {}, .extent = render_target.extent },
		.layerCount = 1,
		.viewMask = 0,
		.colorAttachmentCount = 1,
		.pColorAttachments = &color_attachment,
		.pDepthAttachment = nullptr,
		.pStencilAttachment = nullptr,
	};
	
	vkCmdBeginRendering(command_buffer, &rendering_info);
}

void FrameContext::record_end(const RenderTarget& render_target) {
	VkResult result;
	
	vkCmdEndRendering(command_buffer);

	VkImageMemoryBarrier barrier{
		.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
		.pNext = nullptr,
		.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
		.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT,
		.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
		.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
		.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.image = render_target.image,
		.subresourceRange = {
			.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
			.baseMipLevel = 0,
			.levelCount = 1,
			.baseArrayLayer = 0,
			.layerCount = 1
		}
	};

	vkCmdPipelineBarrier(
		command_buffer,
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
		0,
		0, nullptr,
		0, nullptr,
		1, &barrier
	);

	result = vkEndCommandBuffer(command_buffer);
	assert(result == VK_SUCCESS);
}

void FrameContext::record(GraphicsPipeline& pipeline, RenderTarget render_target) {
	(void) render_target;
	vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.pipeline);

	for (int64_t i = 0; i < 1; ++i) {
		vkCmdBindDescriptorSets(
			command_buffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			pipeline.layout,
			0,
			1,
			&frame_descriptor_set,
			0,
			nullptr
		);

		vkCmdDraw(command_buffer, 4, 1, 0, 0);
	}
}

void FrameContext::update_frame_uniform(FrameUniformData data) {
    VkResult result;

    void* mapped;
    result = vkMapMemory(VulkanInstance::singleton()->device, frame_uniform_memory, 0, sizeof(data), 0, &mapped);
	assert(result == VK_SUCCESS);
    memcpy(mapped, &data, sizeof(data));
    vkUnmapMemory(VulkanInstance::singleton()->device, frame_uniform_memory);
}

}