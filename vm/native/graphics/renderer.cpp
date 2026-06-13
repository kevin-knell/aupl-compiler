#include "renderer.hpp"

#include <numeric>

#include "vulkan.hpp"
#include "graphics_pipeline.hpp"
#include "shader_loader.hpp"
#include "color_rect.hpp"
#include "vertex.hpp"
#include "vec2.hpp"
#include "mat4.hpp"
#include "curve_2d.hpp"
#include "shared.hpp"
#include "canvas_item_rid.hpp"
#include "render_command.hpp"
#include "texture_rid.hpp"

namespace auplib {

void Renderer::register_to_db(vm::ClassDB &db) {
	const int16_t ID = REGISTER_OBJECT_CLASS(Renderer, Object);
	
	REGISTER_CONSTRUCTOR(ID, Renderer(Shared<Viewport> viewport));

	REGISTER_METHOD(ID, Renderer, render, void (Renderer::*)());

	REGISTER_GET_ONLY(ID, Renderer, Shared<Viewport>, viewport);

}

Renderer::Renderer(Shared<Viewport> viewport) : viewport(viewport) {
	VkResult result;

	viewport->resize_raw_obj = reinterpret_cast<void*>(this);
	viewport->on_resize = &on_resize_renderer;

	// create frame contexts
	frames.resize(2);

	// create swapchain
	uint32_t surface_count;
	std::vector<VkSurfaceFormatKHR> surface_formats;

	result = vkGetPhysicalDeviceSurfaceFormatsKHR(VulkanInstance::singleton()->phys_device, viewport->surface, &surface_count, nullptr);
	assert(result == VK_SUCCESS);

	surface_formats.resize(surface_count);

	result = vkGetPhysicalDeviceSurfaceFormatsKHR(VulkanInstance::singleton()->phys_device, viewport->surface, &surface_count, surface_formats.data());
	assert(result == VK_SUCCESS);

	swapchain = Swapchain(viewport->surface, surface_formats[0].format, viewport->scissor.extent);

	// descriptor pool
	std::vector<VkDescriptorPoolSize> pool_sizes = {
		// Frame Uniform
		VkDescriptorPoolSize{
			.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			.descriptorCount = 100
		},
		// Object Uniform
		VkDescriptorPoolSize{
			.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			.descriptorCount = 100
		}
	};

	VkDescriptorPoolCreateInfo pool_create_info{
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.maxSets = 200,
		.poolSizeCount = static_cast<uint32_t>(pool_sizes.size()),
		.pPoolSizes = pool_sizes.data()
	};
	
	result = vkCreateDescriptorPool(VulkanInstance::singleton()->device, &pool_create_info, nullptr, &VulkanInstance::singleton()->desc_pool);
	assert(result == VK_SUCCESS);
	
	// frame desc set layout
	VkDescriptorSetLayoutBinding frame_uniform_binding{
		.binding = 0,
		.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
		.descriptorCount = 1,
		.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT,
		.pImmutableSamplers = nullptr
	};

	VkDescriptorSetLayoutCreateInfo frame_desc_set_layout_create_info{
    	.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
    	.bindingCount = 1,
    	.pBindings = &frame_uniform_binding
	};
	
	result = vkCreateDescriptorSetLayout(VulkanInstance::singleton()->device, &frame_desc_set_layout_create_info, nullptr, &VulkanInstance::singleton()->desc_set_layout_frame);
	assert(result == VK_SUCCESS);

	// object desc set layout
	VkDescriptorSetLayoutBinding object_uniform_binding{
		.binding = 0,
		.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
		.descriptorCount = 1,
		.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT,
		.pImmutableSamplers = nullptr
	};

	VkDescriptorSetLayoutCreateInfo object_desc_set_layout_create_info{
    	.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
    	.bindingCount = 1,
    	.pBindings = &object_uniform_binding
	};
	
	result = vkCreateDescriptorSetLayout(VulkanInstance::singleton()->device, &object_desc_set_layout_create_info, nullptr, &VulkanInstance::singleton()->desc_set_layout_object);
	assert(result == VK_SUCCESS);

	// sampler desc set layout
	VkDescriptorSetLayoutBinding sampler_uniform_binding{
		.binding = 0,
		.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
		.descriptorCount = 1,
		.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
		.pImmutableSamplers = nullptr
	};

	VkDescriptorSetLayoutCreateInfo sampler_desc_set_layout_create_info{
    	.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
    	.bindingCount = 1,
    	.pBindings = &sampler_uniform_binding
	};
	
	result = vkCreateDescriptorSetLayout(VulkanInstance::singleton()->device, &sampler_desc_set_layout_create_info, nullptr, &VulkanInstance::singleton()->desc_set_layout_sampler);
	assert(result == VK_SUCCESS);

	// frame descriptor sets
	VkDescriptorSetAllocateInfo desc_alloc_info{
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
		.pNext = nullptr,
		.descriptorPool = VulkanInstance::singleton()->desc_pool,
		.descriptorSetCount = 1,
		.pSetLayouts = &VulkanInstance::singleton()->desc_set_layout_frame
	};

	for (FrameContext& f : frames) {
		// allocate desc set
		result = vkAllocateDescriptorSets(VulkanInstance::singleton()->device, &desc_alloc_info, &f.frame_descriptor_set);
		assert(result == VK_SUCCESS);

		// update desc set
		VkDescriptorBufferInfo bufferInfo{
			.buffer = f.frame_uniform_buffer,
			.offset = 0,
			.range = sizeof(FrameContext::FrameUniformData)
		};
		
		VkWriteDescriptorSet write{
			.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
			.pNext = nullptr,
			.dstSet = f.frame_descriptor_set,
			.dstBinding = 0,
			.dstArrayElement = 0,
			.descriptorCount = 1,
			.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			.pImageInfo = nullptr,
			.pBufferInfo = &bufferInfo,
			.pTexelBufferView = nullptr
		};

		vkUpdateDescriptorSets(VulkanInstance::singleton()->device, 1, &write, 0, nullptr);
	}

	// pipeline layout
	std::vector<VkDescriptorSetLayout> set_layouts = {
		VulkanInstance::singleton()->desc_set_layout_frame,
		VulkanInstance::singleton()->desc_set_layout_object,
		VulkanInstance::singleton()->desc_set_layout_sampler
	};

	VkPipelineLayoutCreateInfo pipeline_layout_create_info{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.setLayoutCount = static_cast<uint32_t>(set_layouts.size()),
		.pSetLayouts = set_layouts.data(),
		.pushConstantRangeCount = 0,
		.pPushConstantRanges = nullptr
	};
	
	result = vkCreatePipelineLayout(
		VulkanInstance::singleton()->device,
		&pipeline_layout_create_info,
		nullptr,
		&pipeline_layout
	);

	std::vector<VkPipelineShaderStageCreateInfo> shader_stages = {
		ShaderLoader::load_shader("shaders/shader.vert.spv", VK_SHADER_STAGE_VERTEX_BIT),
		ShaderLoader::load_shader("shaders/shader.frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT)
	};

	test_pipeline = GraphicsPipeline(pipeline_layout, shader_stages);

	std::vector<VkPipelineShaderStageCreateInfo> shader_stages_bezier = {
		ShaderLoader::load_shader("shaders/bezier.vert.spv", VK_SHADER_STAGE_VERTEX_BIT),
		ShaderLoader::load_shader("shaders/bezier.tesc.spv", VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT),
		ShaderLoader::load_shader("shaders/bezier.tese.spv", VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT),
		ShaderLoader::load_shader("shaders/bezier.frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT)
	};

	bezier_pipeline = GraphicsPipeline(pipeline_layout, shader_stages_bezier);

	// vertex buffer
	VkBufferCreateInfo vertex_buffer_create_info = {
		.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.size = sizeof(Vertex) * 100,
		.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
		.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
		.queueFamilyIndexCount = 0,
		.pQueueFamilyIndices = 0,
	};

	result = vkCreateBuffer(VulkanInstance::singleton()->device, &vertex_buffer_create_info, nullptr, &vertex_buffer);
	assert(result == VK_SUCCESS);

	VkMemoryRequirements mem_req;
	vkGetBufferMemoryRequirements(VulkanInstance::singleton()->device, vertex_buffer, &mem_req);

	VkMemoryAllocateInfo mem_alloc_info = {
		.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
		.pNext = nullptr,
		.allocationSize = mem_req.size,
		.memoryTypeIndex = VulkanInstance::singleton()->findMemoryType(
			mem_req.memoryTypeBits,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
			VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		)
	};

	result = vkAllocateMemory(VulkanInstance::singleton()->device, &mem_alloc_info, nullptr, &vertex_memory);
	assert(result == VK_SUCCESS);

	result = vkBindBufferMemory(VulkanInstance::singleton()->device, vertex_buffer, vertex_memory, 0);
	assert(result == VK_SUCCESS);

	void* raw_data;
	vkMapMemory(VulkanInstance::singleton()->device, vertex_memory, 0, vertex_buffer_create_info.size, 0, &raw_data);
	mapped_vertices = reinterpret_cast<Vertex*>(raw_data);

	// command pool
	VkCommandPoolCreateInfo pool_info{
		.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
		.pNext = nullptr,
		.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
		.queueFamilyIndex = VulkanInstance::singleton()->selected_queue_family_idx
	};

	result = vkCreateCommandPool(VulkanInstance::singleton()->device, &pool_info, nullptr, &command_pool);
	assert(result == VK_SUCCESS);

	// command buffers
	std::vector<VkCommandBuffer> command_buffers;
	command_buffers.resize(frames.size());

	VkCommandBufferAllocateInfo alloc_info{
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		.pNext = nullptr,
		.commandPool = command_pool,
		.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
		.commandBufferCount = static_cast<uint32_t>(command_buffers.size())
	};

	result = vkAllocateCommandBuffers(VulkanInstance::singleton()->device, &alloc_info, command_buffers.data());
	assert(result == VK_SUCCESS);

	for (size_t i = 0; i < frames.size(); ++i) {
		frames[i].command_buffer = command_buffers[i];
		
		FrameContext::FrameUniformData frame_uniform_data{};
		frame_uniform_data.viewport_size = vec2(
			viewport->vk_viewport.width,
			viewport->vk_viewport.height
		);
		
		frames[i].update_frame_uniform(frame_uniform_data);
	}

	current_frame = 0;
}

Renderer::~Renderer() {
	vkDestroyCommandPool(VulkanInstance::singleton()->device, command_pool, nullptr);
}

void Renderer::draw_rect(const RenderCommandRect& cmd, CanvasItemRID ci, FrameContext& frame) {
	assert(!!cmd.tex);

	vkCmdBindPipeline(
		frame.command_buffer,
		VK_PIPELINE_BIND_POINT_GRAPHICS,
		test_pipeline.pipeline);
	
	// unit quad
	mapped_vertices[4] = Vertex{ vec2(0.0, 0.0),	vec3(1.0, 1.0, 1.0), { 0.0, 0.0 } };
	mapped_vertices[5] = Vertex{ vec2(0.0, 1.0),	vec3(1.0, 1.0, 1.0), { 0.0, 1.0 } };
	mapped_vertices[6] = Vertex{ vec2(1.0, 0.0),	vec3(1.0, 1.0, 1.0), { 1.0, 0.0 } };
	mapped_vertices[7] = Vertex{ vec2(1.0, 1.0),	vec3(1.0, 1.0, 1.0), { 1.0, 1.0 } };

	memcpy(
		ci->object_uniform_mapped,
		&ci->object_ubo,
		sizeof(ObjectUBO)
	);

	// descriptor sets
	vkCmdBindDescriptorSets(
		frame.command_buffer,
		VK_PIPELINE_BIND_POINT_GRAPHICS,
		pipeline_layout,
		0,
		1,
		&frame.frame_descriptor_set,
		0,
		nullptr
	);

	vkCmdBindDescriptorSets(
		frame.command_buffer,
		VK_PIPELINE_BIND_POINT_GRAPHICS,
		pipeline_layout,
		1,
		1,
		&ci->object_descriptor_set,
		0,
		nullptr
	);

	VkDescriptorSet sampler_descriptor_set = cmd.tex->get_descriptor_set();

	vkCmdBindDescriptorSets(
		frame.command_buffer,
		VK_PIPELINE_BIND_POINT_GRAPHICS,
		pipeline_layout,
		2,
		1,
		&sampler_descriptor_set,
		0,
		nullptr
	);

	VkDeviceSize offset{ sizeof(Vertex) * 4 };
	vkCmdBindVertexBuffers(frame.command_buffer, 0, 1, &vertex_buffer, &offset);

	vkCmdDraw(
		frame.command_buffer,
		4,
		1,
		0,
		0);
}

void Renderer::draw_curve(const RenderCommandCurve& cmd, CanvasItemRID ci, FrameContext& frame) {
	vkCmdBindPipeline(
		frame.command_buffer,
		VK_PIPELINE_BIND_POINT_GRAPHICS,
		bezier_pipeline.pipeline);

	mapped_vertices[0] = Vertex{ cmd.points[0], vec3(1.0, 1.0, 1.0), { 0.0, 0.0 } };
	mapped_vertices[1] = Vertex{ cmd.points[1], vec3(1.0, 1.0, 1.0), { 0.0, 1.0 } };
	mapped_vertices[2] = Vertex{ cmd.points[2], vec3(1.0, 1.0, 1.0), { 1.0, 0.0 } };
	mapped_vertices[3] = Vertex{ cmd.points[3], vec3(1.0, 1.0, 1.0), { 1.0, 1.0 } };

	memcpy(
		ci->object_uniform_mapped,
		&ci->object_ubo,
		sizeof(ObjectUBO)
	);

	// descriptor sets
	vkCmdBindDescriptorSets(
		frame.command_buffer,
		VK_PIPELINE_BIND_POINT_GRAPHICS,
		pipeline_layout,
		0,
		1,
		&frame.frame_descriptor_set,
		0,
		nullptr
	);

	vkCmdBindDescriptorSets(
		frame.command_buffer,
		VK_PIPELINE_BIND_POINT_GRAPHICS,
		pipeline_layout,
		1,
		1,
		&ci->object_descriptor_set,
		0,
		nullptr
	);

	VkDescriptorSet sampler_descriptor_set = cmd.tex->get_descriptor_set();

	vkCmdBindDescriptorSets(
		frame.command_buffer,
		VK_PIPELINE_BIND_POINT_GRAPHICS,
		pipeline_layout,
		2,
		1,
		&sampler_descriptor_set,
		0,
		nullptr
	);

	VkDeviceSize offset{ 0 };
	vkCmdBindVertexBuffers(frame.command_buffer, 0, 1, &vertex_buffer, &offset);

	vkCmdDraw(
		frame.command_buffer,
		4,
		1,
		0,
		0);
}


CanvasItem* get_canvas_item_root(Node* root) {
	if(CanvasItem* root_ci = Object::cast_to<CanvasItem>(root)) {
		return root_ci;
	}

	List<Shared<Node>> children = root->get_children();

	for (size_t i = 0; i < children.size(); ++i) {
		CanvasItem* ci = get_canvas_item_root(children[i].get());

		if (ci) {
			return ci;
		}
	}

	return nullptr;
}

void Renderer::render() {
	Scene& scene = *viewport->scene;
	CanvasItem* root_ci = get_canvas_item_root(scene.root.get());

	if (!root_ci) {
		// nothing to draw
		return;
	}

	VkResult result;

	// get current frame
	FrameContext& frame = frames[current_frame];

	result = vkWaitForFences(
		VulkanInstance::singleton()->device,
		1,
		&frame.in_flight_fence,
		VK_TRUE,
		UINT64_MAX
    );
	assert(result == VK_SUCCESS);

    result = vkResetFences(
        VulkanInstance::singleton()->device,
        1,
        &frame.in_flight_fence
    );
	assert(result == VK_SUCCESS);

	// acquire image index from swapchain
	uint32_t image_index;
	
	result = vkAcquireNextImageKHR(
		VulkanInstance::singleton()->device,
		swapchain.vk_swapchain,
		std::numeric_limits<uint64_t>::max(),
		frames[current_frame].image_available_semaphore,
		VK_NULL_HANDLE,
		&image_index
	);
	assert(result == VK_SUCCESS);

	const RenderTarget& render_target = swapchain.render_targets[image_index];

	// begin drawing
	frame.record_begin(render_target);
	
	// dynamic states
	VkViewport viewport{
		.x = 0.0f,
		.y = 0.0f,
		.width = static_cast<float>(render_target.extent.width),
		.height = static_cast<float>(render_target.extent.height),
		.minDepth = 0.0f,
		.maxDepth = 1.0f
	};
	vkCmdSetViewport(frame.command_buffer, 0, 1, &viewport);
	
	VkRect2D scissor{
		.offset{},
		.extent = render_target.extent
	};
	vkCmdSetScissor(frame.command_buffer, 0, 1, &scissor);
	
	// draw
	CanvasItemRID_T* current_ci = root_ci->get_canvas_item_rid();

	size_t count = 0;

	while(current_ci) {
		RenderCommand* cmd = current_ci->commands;

		if (!!cmd) {
			switch(cmd->type) {
				case RenderCommand::TYPE_RECT: {
					RenderCommandRect* cmd_rect = static_cast<RenderCommandRect*>(cmd);
					draw_rect(*cmd_rect, current_ci, frame);
					break;
				}
				case RenderCommand::TYPE_CURVE: {
					RenderCommandCurve* cmd_curve = static_cast<RenderCommandCurve*>(cmd);
					draw_curve(*cmd_curve, current_ci, frame);
					break;
				}
				default: {
					std::cerr << "invalid render command: " << cmd->type << std::endl;
					abort();
				}
			}
		}

		current_ci = current_ci->next;
		++count;
	}

	// end drawing
	frame.record_end(render_target);

	// submit
	VkPipelineStageFlags stage_flags[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

	VkSubmitInfo submit_info{
		.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
		.pNext = nullptr,
		.waitSemaphoreCount = 1,
		.pWaitSemaphores = &frame.image_available_semaphore,
		.pWaitDstStageMask = stage_flags,
		.commandBufferCount = 1,
		.pCommandBuffers = &frame.command_buffer,
		.signalSemaphoreCount = 1,
		.pSignalSemaphores = &frame.render_finished_semaphore,
	};

	result = vkQueueSubmit(VulkanInstance::singleton()->queue, 1, &submit_info, frame.in_flight_fence);
	assert(result == VK_SUCCESS);

	// present
	VkPresentInfoKHR present_info{
		.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
		.pNext = nullptr,
		.waitSemaphoreCount = 1,
		.pWaitSemaphores = &frame.render_finished_semaphore,
		.swapchainCount = 1,
		.pSwapchains = &swapchain.vk_swapchain,
		.pImageIndices = &image_index,
		.pResults = nullptr
	};

	result = vkQueuePresentKHR(VulkanInstance::singleton()->queue, &present_info);
	assert(result == VK_SUCCESS);

	current_frame = (current_frame + 1) % 2;
}

void Renderer::on_resize() {
	vkDeviceWaitIdle(VulkanInstance::singleton()->device);

	swapchain.recreate(viewport->scissor.extent);

	for (size_t i = 0; i < frames.size(); ++i) {
		FrameContext::FrameUniformData frame_uniform_data{};
		
		frame_uniform_data.viewport_size = vec2(
			viewport->vk_viewport.width,
			viewport->vk_viewport.height
		);

		frames[i].update_frame_uniform(frame_uniform_data);
	}
}

void on_resize_renderer(void* raw_obj) {
	Renderer& renderer = *reinterpret_cast<Renderer*>(raw_obj);
	renderer.on_resize();
}

}