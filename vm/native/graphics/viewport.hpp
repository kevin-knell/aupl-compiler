#pragma once
#include "object.hpp"

#include <vulkan/vulkan.h>
#include <vector>
#include <functional>

#include "scene.hpp"
#include "shared.hpp"

namespace auplib
{

class Viewport : Object {
public:
	VkSurfaceKHR surface;

	VkViewport vk_viewport;
	VkRect2D scissor;

	Shared<Scene> scene;

	void* resize_raw_obj;
	std::function<void(void* raw_obj)> on_resize;
	
	static void register_to_db(vm::ClassDB &db);

	Viewport(const uint32_t width, const uint32_t height);

	void resize(const uint32_t width, const uint32_t height);
};

} // namespace auplib
