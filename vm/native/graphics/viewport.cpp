#include "viewport.hpp"

namespace auplib {

void Viewport::register_to_db(vm::ClassDB &db) {
	const int16_t ID = REGISTER_CLASS(Viewport);

	REGISTER_VARIABLE(ID, Shared<Scene>, scene);
}

Viewport::Viewport(const uint32_t width, const uint32_t height) {
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

void Viewport::resize(const uint32_t width, const uint32_t height) {
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

	on_resize(resize_raw_obj);
}

}