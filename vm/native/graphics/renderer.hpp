#pragma once

#include <vulkan/vulkan.h>
#include "viewport.hpp"
#include "object.hpp"
#include "shared.hpp"
#include "swapchain.hpp"
#include "frame_context.hpp"

namespace auplib
{

struct Vertex;

class Renderer : public Object {
public:
	Shared<Viewport> viewport;

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

	void draw_node(Shared<Node> node, FrameContext& frame);
	void render();

	void on_resize();
};

void on_resize_renderer(void* raw_obj);

} // namespace auplib
