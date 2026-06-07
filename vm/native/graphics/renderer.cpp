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

namespace auplib {

void Renderer::register_to_db(vm::ClassDB &db) {
	const int16_t ID = REGISTER_CLASS(Renderer);
	
	REGISTER_CONSTRUCTOR(ID, Renderer(Shared<Viewport> viewport));

	REGISTER_METHOD(ID, Renderer, render, void (Renderer::*)());

	REGISTER_VARIABLE(ID, Shared<Viewport>, viewport);

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

	result = vkGetPhysicalDeviceSurfaceFormatsKHR(vulkan_instance.phys_device, viewport->surface, &surface_count, nullptr);
	assert(result == VK_SUCCESS);

	surface_formats.resize(surface_count);

	result = vkGetPhysicalDeviceSurfaceFormatsKHR(vulkan_instance.phys_device, viewport->surface, &surface_count, surface_formats.data());
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
	
	result = vkCreateDescriptorPool(vulkan_instance.device, &pool_create_info, nullptr, &vulkan_instance.desc_pool);
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
	
	result = vkCreateDescriptorSetLayout(vulkan_instance.device, &frame_desc_set_layout_create_info, nullptr, &vulkan_instance.desc_set_layout_frame);
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
	
	result = vkCreateDescriptorSetLayout(vulkan_instance.device, &object_desc_set_layout_create_info, nullptr, &vulkan_instance.desc_set_layout_object);
	assert(result == VK_SUCCESS);

	// frame descriptor sets
	VkDescriptorSetAllocateInfo desc_alloc_info{
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
		.pNext = nullptr,
		.descriptorPool = vulkan_instance.desc_pool,
		.descriptorSetCount = 1,
		.pSetLayouts = &vulkan_instance.desc_set_layout_frame
	};

	for (FrameContext& f : frames) {
		// allocate desc set
		result = vkAllocateDescriptorSets(vulkan_instance.device, &desc_alloc_info, &f.frame_descriptor_set);
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

		vkUpdateDescriptorSets(vulkan_instance.device, 1, &write, 0, nullptr);
	}

	// pipeline layout
	std::vector<VkDescriptorSetLayout> set_layouts = {
		vulkan_instance.desc_set_layout_frame,
		vulkan_instance.desc_set_layout_object
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
		vulkan_instance.device,
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

	result = vkCreateBuffer(vulkan_instance.device, &vertex_buffer_create_info, nullptr, &vertex_buffer);
	assert(result == VK_SUCCESS);

	VkMemoryRequirements mem_req;
	vkGetBufferMemoryRequirements(vulkan_instance.device, vertex_buffer, &mem_req);

	VkMemoryAllocateInfo mem_alloc_info = {
		.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
		.pNext = nullptr,
		.allocationSize = mem_req.size,
		.memoryTypeIndex = vulkan_instance.findMemoryType(
			mem_req.memoryTypeBits,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
			VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		)
	};

	result = vkAllocateMemory(vulkan_instance.device, &mem_alloc_info, nullptr, &vertex_memory);
	assert(result == VK_SUCCESS);

	result = vkBindBufferMemory(vulkan_instance.device, vertex_buffer, vertex_memory, 0);
	assert(result == VK_SUCCESS);

	void* raw_data;
	vkMapMemory(vulkan_instance.device, vertex_memory, 0, vertex_buffer_create_info.size, 0, &raw_data);
	mapped_vertices = reinterpret_cast<Vertex*>(raw_data);

	// command pool
	VkCommandPoolCreateInfo pool_info{
		.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
		.pNext = nullptr,
		.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
		.queueFamilyIndex = vulkan_instance.selected_queue_family_idx
	};

	result = vkCreateCommandPool(vulkan_instance.device, &pool_info, nullptr, &command_pool);
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

	result = vkAllocateCommandBuffers(vulkan_instance.device, &alloc_info, command_buffers.data());
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
	vkDestroyCommandPool(vulkan_instance.device, command_pool, nullptr);
}

void Renderer::draw_node(Shared<Node> node, FrameContext& frame) {
	if (ColorRect* r = dynamic_cast<ColorRect*>(node.get())) {
		vkCmdBindPipeline(
			frame.command_buffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			test_pipeline.pipeline);
		
		mapped_vertices[4] = Vertex{ vec2(0.0, 0.0), vec3(1.0, 1.0, 1.0) };
		mapped_vertices[5] = Vertex{ vec2(0.0, 1.0), vec3(1.0, 1.0, 1.0) };
		mapped_vertices[6] = Vertex{ vec2(1.0, 0.0), vec3(1.0, 1.0, 1.0) };
		mapped_vertices[7] = Vertex{ vec2(1.0, 1.0), vec3(1.0, 1.0, 1.0) };

		mat4 model = {
			.rows = {
				{ r->size.x, 0.0, 0.0, 0.0 },
				{ 0.0, r->size.y, 0.0, 0.0 },
				{ 0.0, 0.0, 1.0, 0.0 },
				{ r->position.x, r->position.y, 0.0, 1.0 },
			}
		};

		r->object_data.model = model;

		memcpy(
			r->object_uniform_mapped,
			&r->object_data,
			sizeof(CanvasItem::ObjectUniformData)
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
			&r->object_descriptor_set,
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
	} else if (Curve2D* curve_2d = dynamic_cast<Curve2D*>(node.get())) {
		vkCmdBindPipeline(
        	frame.command_buffer,
        	VK_PIPELINE_BIND_POINT_GRAPHICS,
        	bezier_pipeline.pipeline);

		mat4 model = {
			.rows = {
				{ 1.0, 0.0, 0.0, 0.0 },
				{ 0.0, 1.0, 0.0, 0.0 },
				{ 0.0, 0.0, 1.0, 0.0 },
				{ 0.0, 0.0, 0.0, 1.0 },
			}
		};

		assert(curve_2d->points.size() == 4);

		mapped_vertices[0] = Vertex{ curve_2d->points[0], vec3(1.0, 1.0, 1.0) };
		mapped_vertices[1] = Vertex{ curve_2d->points[1], vec3(1.0, 1.0, 1.0) };
		mapped_vertices[2] = Vertex{ curve_2d->points[2], vec3(1.0, 1.0, 1.0) };
		mapped_vertices[3] = Vertex{ curve_2d->points[3], vec3(1.0, 1.0, 1.0) };


		curve_2d->object_data.model = model;

		memcpy(
			curve_2d->object_uniform_mapped,
			&curve_2d->object_data,
			sizeof(CanvasItem::ObjectUniformData)
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
			&curve_2d->object_descriptor_set,
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

	for (size_t i = 0; i < node->children.size(); ++i) {
		draw_node(node->children[i], frame);
	}
}

void Renderer::render() {
	VkResult result;

	FrameContext& frame = frames[current_frame];

	result = vkWaitForFences(
		vulkan_instance.device,
		1,
		&frame.in_flight_fence,
		VK_TRUE,
		UINT64_MAX
    );
	assert(result == VK_SUCCESS);

    result = vkResetFences(
        vulkan_instance.device,
        1,
        &frame.in_flight_fence
    );
	assert(result == VK_SUCCESS);

	uint32_t image_index;
	
	result = vkAcquireNextImageKHR(
		vulkan_instance.device,
		swapchain.vk_swapchain,
		std::numeric_limits<uint64_t>::max(),
		frames[current_frame].image_available_semaphore,
		VK_NULL_HANDLE,
		&image_index
	);
	assert(result == VK_SUCCESS);

	const RenderTarget& render_target = swapchain.render_targets[image_index];
	
	frame.record_begin(render_target);

	// draw scene
	Scene& scene = *viewport->scene;

	// TEMPORARY CODE START
	
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
	draw_node(scene.root, frame);

	// TEMPORARY CODE END

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

	result = vkQueueSubmit(vulkan_instance.queue, 1, &submit_info, frame.in_flight_fence);
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

	result = vkQueuePresentKHR(vulkan_instance.queue, &present_info);
	assert(result == VK_SUCCESS);

	current_frame = (current_frame + 1) % 2;
}

void Renderer::on_resize() {
	vkDeviceWaitIdle(vulkan_instance.device);

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