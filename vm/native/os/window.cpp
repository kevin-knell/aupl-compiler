#include "window.hpp"
#include "vulkan.hpp"
#include <cstdint>
#include <numeric>
#include "class_db.hpp"
#include "input.hpp"

namespace auplib {

std::map<GLFWwindow*, Window*> Window::active_windows;

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
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
	glfw_window = glfwCreateWindow(width, height, name, nullptr, nullptr);
	glfwCreateWindowSurface(vulkan_instance.instance, glfw_window, nullptr, &viewport->surface);
	glfwSetWindowSizeCallback(glfw_window, &window_size_callback);

	Input::window = glfw_window;
	active_windows[glfw_window] = this;
}

Window::~Window() {
	glfwDestroyWindow(glfw_window);
	glfwTerminate();
}

bool Window::should_close() {
	return glfwWindowShouldClose(glfw_window);
}

void Window::on_resize(int width, int height) {
	assert(width > 0);
	assert(height > 0);
	viewport->resize(width, height);
	
}

void window_size_callback(GLFWwindow* window, int width, int height) {
	Window::active_windows[window]->on_resize(width, height);
}

}