#include "window.hpp"
#include "vulkan.hpp"
#include <cstdint>
#include <numeric>
#include "class_db.hpp"
#include "input.hpp"

namespace auplib {

void Window::register_to_db(vm::ClassDB &db) {
	const int16_t ID = REGISTER_CLASS(Window);
	
	REGISTER_CONSTRUCTOR(ID, Window(const uint32_t width, const uint32_t height, const String name));

	REGISTER_STATIC_METHOD(ID, Window, poll_events, void (*)());
	REGISTER_METHOD(ID, Window, should_close, bool (Window::*)());
	REGISTER_VARIABLE(ID, Shared<Viewport>, viewport);
}

void Window::poll_events() {
	glfwPollEvents();
}

Window::Window(const uint32_t width, const uint32_t height, const String name)
		: width(width), height(height), name(name) {
	viewport = Shared<Viewport>::make(width, height);
	
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	glfw_window = glfwCreateWindow(width, height, name, nullptr, nullptr);
	glfwCreateWindowSurface(vulkan_instance.instance, glfw_window, nullptr, &viewport->surface);

	Input::window = glfw_window;
}

Window::~Window() {
	glfwDestroyWindow(glfw_window);
	glfwTerminate();
}

bool Window::should_close() {
	return glfwWindowShouldClose(glfw_window);
}

}