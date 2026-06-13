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

public:
	Shared<Viewport> viewport;

	VkBuffer instance_buffer;
	VkDeviceMemory instance_memory;
	void* mapped_instance_data;

	VkBuffer vertex_buffer;
	VkDeviceMemory vertex_memory;
	Vertex* mapped_vertices;

	VkCommandPool command_pool;
	std::vector<FrameContext> frames;
	uint32_t current_frame;

	GraphicsPipeline test_pipeline;
	GraphicsPipeline bezier_pipeline;
	VkPipelineLayout pipeline_layout;

	Swapchain swapchain;
	
	static void register_to_db(vm::ClassDB &db);
	
	Renderer(Shared<Viewport> viewport);
	~Renderer();

	void draw_rect(const RenderCommandRect& cmd, CanvasItemRID ci, FrameContext& frame);
	void draw_curve(const RenderCommandCurve& cmd, CanvasItemRID ci, FrameContext& frame);
	
	void render();

	void on_resize();

	Shared<Viewport> get_viewport() const { return viewport; }
};

void on_resize_renderer(void* raw_obj);

} // namespace auplib
