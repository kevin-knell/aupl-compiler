#pragma once
#include "object.hpp"

#include <vulkan/vulkan.h>
#include <vector>

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
	
	static void register_to_db(vm::ClassDB &db);

	Viewport(const uint32_t width, const uint32_t height) {
		vk_viewport = VkViewport{
			.x = 0,
			.y = 0,
			.width = static_cast<float>(width),
			.height = static_cast<float>(height),
			.minDepth = 0.0f,
			.maxDepth = 1.0f
		};

		scissor = VkRect2D{
			.offset = { .x = 0, .y = 0 },
			.extent = { .width = width, .height = height }
		};
	}
};

} // namespace auplib
