#pragma once

#include <vulkan/vulkan.h>
#include "viewport.hpp"
#include "object.hpp"
#include "shared.hpp"
#include "swapchain.hpp"
#include "frame_context.hpp"
#include "rid_forward_decl.hpp"

namespace auplib
{

struct Vertex;
struct RenderCommandRect;
struct RenderCommandCurve;

class Renderer : public Object {
private:
	struct InputBuffer {
		VkBuffer buffer;
		VkDeviceMemory memory;
		void* mapped_memory;
	};

	InputBuffer create_input_buffer();

public:
	Renderer(Shared<Viewport> viewport);
	~Renderer();

	static void register_to_db(vm::ClassDB &db);

private:
	size_t instance_index;
	
public:
	Shared<Viewport> viewport;

	InputBuffer instance_buffer;
	InputBuffer vertex_buffer;

	VkCommandPool command_pool;
	std::vector<FrameContext> frames;
	uint32_t current_frame;

	GraphicsPipeline test_pipeline;
	GraphicsPipeline bezier_pipeline;
	VkPipelineLayout pipeline_layout;

	Swapchain swapchain;

	void draw_rect(const RenderCommandRect& cmd, CanvasItemRID ci, FrameContext& frame);
	void draw_curve(const RenderCommandCurve& cmd, CanvasItemRID ci, FrameContext& frame);
	
	void render();

	void on_resize();

	Shared<Viewport> get_viewport() const { return viewport; }
};

void on_resize_renderer(void* raw_obj);

} // namespace auplib
