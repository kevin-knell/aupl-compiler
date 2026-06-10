#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <map>

#include "object.hpp"
#include "string.hpp"
#include "list.hpp"
#include "shared.hpp"
#include "viewport.hpp"

namespace auplib
{

void window_size_callback(GLFWwindow* window, int width, int height);

class Window : public Object {
public:
	GLFWwindow* glfw_window;
	Shared<Viewport> viewport;
	
	uint32_t width;
	uint32_t height;
	String name;

public:
	static std::map<GLFWwindow*, Window*> active_windows;

	static void register_to_db(vm::ClassDB &db);
	static void poll_events();

	Window() = delete;
	Window(const uint32_t width, const uint32_t height, const String name);
	~Window();

	bool should_close();

	void on_resize(int width, int height);

	Shared<Viewport> get_viewport() const { return viewport; }
};
	
} // namespace auplib
