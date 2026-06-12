#pragma once
#include <vulkan/vulkan.h>

#include "graphics_pipeline.hpp"
#include "render_target.hpp"
#include "mat4.hpp"
#include "vec3.hpp"
#include "vec2.hpp"
#include "macros.hpp"

namespace auplib
{

struct FrameContext {
	struct FrameUniformData {
		mat4 view;
		mat4 projection;
		mat4 view_projection;

		mat4 inv_view;
		mat4 inv_projection;
		mat4 inv_view_projection;

		vec3 camera_position;
		float time;

		vec2 viewport_size;
		vec2 inv_viewport_size;

		float delta_time;
	};

	VkCommandBuffer command_buffer;

	VkDescriptorSet frame_descriptor_set;

	VkBuffer frame_uniform_buffer;
	VkDeviceMemory frame_uniform_memory;

	VkSemaphore image_available_semaphore;
	VkSemaphore render_finished_semaphore;
	VkFence in_flight_fence;

	FrameContext();
	COPY_DELETE(FrameContext)
	MOVE_DEFAULT(FrameContext)

	void record_begin(const RenderTarget& render_target);
	void record_end(const RenderTarget& render_target);

	void record(GraphicsPipeline& pipeline, RenderTarget render_target);
	void update_frame_uniform(FrameUniformData data);
};

} // namespace auplib
