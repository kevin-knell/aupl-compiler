#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "object.hpp"
#include "string.hpp"
#include "color_rect.hpp"
#include "list.hpp"

namespace auplib
{

class Window : public Object {
public:
	struct GlobalData {
		float screen_size[2];
	};

	GLFWwindow* glfw_window;
	VkSurfaceKHR surface;
	VkSwapchainKHR swapchain;
	std::vector<VkImageView> swapchain_image_views;
	uint32_t image_count;
	VkBuffer buffer;
	VkDeviceMemory memory;
	VkDescriptorSet descriptor_set;
	VkPipelineLayout pipeline_layout;
	VkPipeline pipeline;
	VkFramebuffer* framebuffers;
	VkRenderPass render_pass;
	VkCommandBuffer* command_buffers;
	VkSemaphore image_available_semaphore;
	VkSemaphore render_finished_semaphore;
	VkFence in_flight_fence;

	VkViewport viewport;
	VkRect2D scissor;

	uint32_t width;
	uint32_t height;
	String name;

	List<ColorRect> color_rects;

public:
	Window() = delete;
	Window(const unsigned int width, const unsigned int height, const String name);
	~Window();

	bool should_close() {
		return glfwWindowShouldClose(glfw_window);
	}

	void record(uint32_t& image_index);
	void redraw();
	void update_global_uniform();
};
	
} // namespace auplib
