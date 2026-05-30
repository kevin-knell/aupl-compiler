#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "object.hpp"
#include "string.hpp"
#include "color_rect.hpp"
#include "list.hpp"
#include "shared.hpp"
#include "viewport.hpp"

namespace auplib
{

class Window : public Object {
public:
	struct GlobalData {
		float screen_size[2];
	};

	GLFWwindow* glfw_window;
	Shared<Viewport> viewport;
	
	uint32_t width;
	uint32_t height;
	String name;

public:
	static void register_to_db(vm::ClassDB &db);
	static void poll_events();

	Window() = delete;
	Window(const uint32_t width, const uint32_t height, const String name);
	~Window();

	bool should_close();
};
	
} // namespace auplib
