#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "object.hpp"
#include "string.hpp"
#include "color_rect.hpp"
#include "list.hpp"
#include "shared.hpp"

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

	List<Shared<ColorRect>> color_rects;

public:
	static void register_to_db(vm::ClassDB &db);
	static void poll_events() {
		glfwPollEvents();
	}

	Window() = delete;
	Window(const uint32_t width, const uint32_t height, const String name);
	~Window();

	bool should_close() {
		return glfwWindowShouldClose(glfw_window);
	}

	void record(uint32_t& image_index);
	void redraw();
	void update_global_uniform();

	void add(Shared<ColorRect> rect);
};
	
} // namespace auplib
