#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "object.hpp"
#include "string.hpp"

namespace auplib
{

class Window : public Object {
public:
	GLFWwindow* glfw_window;
	VkSurfaceKHR surface;
	VkSwapchainKHR swapchain;
	uint32_t image_count;
	VkCommandBuffer* command_buffers;
	VkSemaphore image_available_semaphore;
	VkSemaphore render_finished_semaphore;

	VkViewport viewport;
	VkRect2D scissor;

	uint32_t width;
	uint32_t height;
	String name;

public:
	Window() = delete;
	Window(const unsigned int width, const unsigned int height, const String name);
	~Window();

	bool should_close() {
		return glfwWindowShouldClose(glfw_window);
	}

	void redraw();
};
	
} // namespace auplib
