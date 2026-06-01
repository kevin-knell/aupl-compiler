#pragma once
#include <vulkan/vulkan.h>

#include "graphics_pipeline.hpp"
#include "render_target.hpp"

namespace auplib
{

struct FrameContext {
	struct GlobalData {
		float screen_size[2];
	};

	VkCommandBuffer command_buffer;

	VkDescriptorSet global_descriptor_set;

	VkBuffer global_uniform_buffer;
	VkDeviceMemory global_uniform_memory;

	VkSemaphore image_available_semaphore;
	VkSemaphore render_finished_semaphore;
	VkFence in_flight_fence;

	FrameContext();

	FrameContext(const FrameContext&) = delete;
	FrameContext& operator=(const FrameContext&) = delete;

	FrameContext(FrameContext&&) = default;
	FrameContext& operator=(FrameContext&&) = default;

	void record_begin(const RenderTarget& render_target);
	void record_end(const RenderTarget& render_target);

	void record(GraphicsPipeline& pipeline, RenderTarget render_target);
	void update_global_uniform(GlobalData data);
};

} // namespace auplib
