#include "window.hpp"
#include "vulkan.hpp"
#include <cstdint>
#include <numeric>
#include "class_db.hpp"

namespace auplib {

void Window::register_to_db(vm::ClassDB &db) {
	const int16_t ID = REGISTER_CLASS(Window);
	
	REGISTER_CONSTRUCTOR(ID, Window(const uint32_t width, const uint32_t height, const String name));

	REGISTER_STATIC_METHOD(ID, Window, poll_events, void (*)());

	REGISTER_METHOD(ID, Window, redraw, void (Window::*)());
	REGISTER_METHOD(ID, Window, should_close, bool (Window::*)());
	REGISTER_METHOD(ID, Window, add, void (Window::*)(Shared<ColorRect> r));
}

Window::Window(const uint32_t width, const uint32_t height, const String name)
		: width(width), height(height), name(name) {
	viewport = VkViewport{
		.x = 0, .y = 0,
		.width = static_cast<float>(width), .height = static_cast<float>(height),
		.minDepth = 0.0f, .maxDepth = 1.0f
	};

	scissor = VkRect2D{ 0, 0, width, height };
	
	init_vulkan();
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	glfw_window = glfwCreateWindow(width, height, name, nullptr, nullptr);
	glfwCreateWindowSurface(vk_instance, glfw_window, nullptr, &surface);
	create_swapchain(*this);
}

Window::~Window() {
	glfwDestroyWindow(glfw_window);
	glfwTerminate();
}

void Window::record(uint32_t& image_index) {
	VkResult result;

	vkResetCommandBuffer(command_buffers[image_index], 0);

	// command buffer begin
	VkCommandBufferBeginInfo cmd_buffer_begin_info{
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		.pNext = nullptr,
		.flags = 0,
		.pInheritanceInfo = nullptr,
	};

	result = vkBeginCommandBuffer(command_buffers[image_index], &cmd_buffer_begin_info);
	assert(result == VK_SUCCESS);

	VkClearValue clear_value{ 0.0f, 0.0f, 0.0f, 1.0f };

	VkRenderPassBeginInfo render_pass_begin_info{
		.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
		.pNext = nullptr,
		.renderPass = render_pass,
		.framebuffer = framebuffers[image_index],
		.renderArea = scissor,
		.clearValueCount = 1,
		.pClearValues = &clear_value,
	};

	vkCmdBeginRenderPass(command_buffers[image_index], &render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);

	vkCmdBindPipeline(command_buffers[image_index], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

	for (int64_t i = 0; i < color_rects.size(); ++i) {
		ColorRect r = *color_rects[i];

		PushConstant push{
			{static_cast<float>(r.position.x), static_cast<float>(r.position.y)},
			{static_cast<float>(r.size.x), static_cast<float>(r.size.y)},
			{r.color}
		};
		
		vkCmdPushConstants(
			command_buffers[image_index],
			pipeline_layout,
			VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
			0,
			sizeof(push),
			&push
		);

		vkCmdBindDescriptorSets(
			command_buffers[image_index],
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			pipeline_layout,
			0,
			1,
			&descriptor_set,
			0,
			nullptr
		);

		vkCmdDraw(command_buffers[image_index], 4, 1, 0, 0);
	}

	vkCmdEndRenderPass(command_buffers[image_index]);

	result = vkEndCommandBuffer(command_buffers[image_index]);
	assert(result == VK_SUCCESS);
}

void Window::redraw() {
	vkWaitForFences(
        vk_device,
        1,
        &in_flight_fence,
        VK_TRUE,
        UINT64_MAX
    );

    vkResetFences(
        vk_device,
        1,
        &in_flight_fence
    );

	uint32_t image_index;
	VkResult result;

	result = vkAcquireNextImageKHR(
		vk_device,
		swapchain,
		std::numeric_limits<uint64_t>::max(),
		image_available_semaphore,
		VK_NULL_HANDLE,
		&image_index
	);
	assert(result == VK_SUCCESS);

	record(image_index);

	VkPipelineStageFlags stage_flags[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

	VkSubmitInfo submit_info{
		.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
		.pNext = nullptr,
		.waitSemaphoreCount = 1,
		.pWaitSemaphores = &image_available_semaphore,
		.pWaitDstStageMask = stage_flags,
		.commandBufferCount = 1,
		.pCommandBuffers = &(command_buffers[image_index]),
		.signalSemaphoreCount = 1,
		.pSignalSemaphores = &render_finished_semaphore,
	};

	result = vkQueueSubmit(vk_queue, 1, &submit_info, in_flight_fence);
	assert(result == VK_SUCCESS);

	VkPresentInfoKHR present_info{
		.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
		.pNext = nullptr,
		.waitSemaphoreCount = 1,
		.pWaitSemaphores = &render_finished_semaphore,
		.swapchainCount = 1,
		.pSwapchains = &swapchain,
		.pImageIndices = &image_index,
		.pResults = &result
	};
	assert(result == VK_SUCCESS);

	result = vkQueuePresentKHR(vk_queue, &present_info);
	assert(result == VK_SUCCESS);
}

void Window::update_global_uniform() {
    GlobalData data;
    data.screen_size[0] = static_cast<float>(width);
    data.screen_size[1] = static_cast<float>(height);

    void* mapped;
    vkMapMemory(vk_device, memory, 0, sizeof(data), 0, &mapped);
    memcpy(mapped, &data, sizeof(data));
    vkUnmapMemory(vk_device, memory);
}

void Window::add(Shared<ColorRect> rect) {
	color_rects.push(rect);
}
}