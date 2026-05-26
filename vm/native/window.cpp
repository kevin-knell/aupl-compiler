#include "window.hpp"
#include "vulkan.hpp"
#include <cstdint>
#include <numeric>

namespace auplib {

Window::Window(const unsigned int width, const unsigned int height, const String name)
		: width(width), height(height), name(name) {
	viewport = VkViewport{
		.x = 0, .y = 0,
		.width = static_cast<float>(width), .height = static_cast<float>(height),
		.minDepth = 0.0f, .maxDepth = 1.0f
	};

	scissor = VkRect2D{ 0, 0, width, height };
	
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

void Window::redraw() {
	uint32_t image_index;
	VkResult result;

	result = vkAcquireNextImageKHR(vk_device, swapchain, std::numeric_limits<uint64_t>::max(), image_available_semaphore, VK_NULL_HANDLE, &image_index);
	assert(result == VK_SUCCESS);

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

	result = vkQueueSubmit(vk_queue, 1, &submit_info, VK_NULL_HANDLE);
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

}