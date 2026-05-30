#include "renderer.hpp"

#include <numeric>

#include "vulkan.hpp"
#include "graphics_pipeline.hpp"
#include "shader_loader.hpp"
#include "color_rect.hpp"

namespace auplib {
void Renderer::register_to_db(vm::ClassDB &db) {
	const int16_t ID = REGISTER_CLASS(Renderer);
	
	REGISTER_CONSTRUCTOR(ID, Renderer(Shared<Viewport> viewport));

	REGISTER_METHOD(ID, Renderer, render, void (Renderer::*)());

	REGISTER_VARIABLE(ID, Shared<Viewport>, viewport);

}

Renderer::Renderer(Shared<Viewport> viewport) : viewport(viewport) {
	VkResult result;

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

	std::cout << "print formats" << std::endl;
	for (auto format : surface_formats) {
		std::cout << format.format << std::endl;
	}

	swapchain = Swapchain(viewport->surface, surface_formats[0].format, viewport->scissor.extent);

	std::cout << "desc pool" << std::endl;

	// descriptor pool
	std::array<VkDescriptorPoolSize, 1> pool_sizes = {
		// Global Uniform
		VkDescriptorPoolSize{
			.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			.descriptorCount = 100
		}
	};

	VkDescriptorPoolCreateInfo pool_create_info{
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.maxSets = 100,
		.poolSizeCount = pool_sizes.size(),
		.pPoolSizes = pool_sizes.data()
	};
	
	result = vkCreateDescriptorPool(vulkan_instance.device, &pool_create_info, nullptr, &desc_pool);
	assert(result == VK_SUCCESS);
	
	// descriptor set layout
	VkDescriptorSetLayoutBinding global_uniform_binding{
		.binding = 0,
		.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
		.descriptorCount = 1,
		.stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
		.pImmutableSamplers = nullptr
	};

	VkDescriptorSetLayoutCreateInfo desc_set_layout_create_info{
    	.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
    	.bindingCount = 1,
    	.pBindings = &global_uniform_binding
	};
	
	result = vkCreateDescriptorSetLayout(vulkan_instance.device, &desc_set_layout_create_info, nullptr, &desc_set_layout);
	assert(result == VK_SUCCESS);

	// descriptor sets
	VkDescriptorSetAllocateInfo desc_alloc_info{
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
		.pNext = nullptr,
		.descriptorPool = desc_pool,
		.descriptorSetCount = 1,
		.pSetLayouts = &desc_set_layout
	};

	for (FrameContext& f : frames) {
		// allocate desc set
		result = vkAllocateDescriptorSets(vulkan_instance.device, &desc_alloc_info, &f.global_descriptor_set);
		assert(result == VK_SUCCESS);

		// update desc set
		VkDescriptorBufferInfo bufferInfo{
			.buffer = f.global_uniform_buffer,
			.offset = 0,
			.range = sizeof(Window::GlobalData)
		};
		
		VkWriteDescriptorSet write{
			.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
			.pNext = nullptr,
			.dstSet = f.global_descriptor_set,
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
	VkPushConstantRange push_const_range{
		.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
		.offset = 0,
		.size = sizeof(PushConstant),
	};

	VkPipelineLayoutCreateInfo pipeline_layout_create_info{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.setLayoutCount = 1,
		.pSetLayouts = &desc_set_layout,
		.pushConstantRangeCount = 1,
		.pPushConstantRanges = &push_const_range
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
		.commandBufferCount = command_buffers.size()
	};

	result = vkAllocateCommandBuffers(vulkan_instance.device, &alloc_info, command_buffers.data());
	assert(result == VK_SUCCESS);

	for (size_t i = 0; i < frames.size(); ++i) {
		frames[i].command_buffer = command_buffers[i];
		frames[i].update_global_uniform();
	}

	current_frame = 0;
}

Renderer::~Renderer() {
	vkDestroyCommandPool(vulkan_instance.device, command_pool, nullptr);
}

void Renderer::draw_node(Shared<Node> node, FrameContext& frame) {
	if (ColorRect* r = dynamic_cast<ColorRect*>(node.get())) {
		PushConstant push{
			{r->position.x, r->position.y},
			{r->size.x, r->size.y},
			{r->color}
		};
		
		vkCmdPushConstants(
			frame.command_buffer,
			pipeline_layout,
			VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
			0,
			sizeof(push),
			&push
		);

		vkCmdBindDescriptorSets(
			frame.command_buffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			pipeline_layout,
			0,
			1,
			&frame.global_descriptor_set,
			0,
			nullptr
		);

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
		swapchain.swapchain,
		std::numeric_limits<uint64_t>::max(),
		frames[current_frame].image_available_semaphore,
		VK_NULL_HANDLE,
		&image_index
	);
	assert(result == VK_SUCCESS);

	const RenderTarget& render_target = swapchain.render_targets[image_index];
	
	frame.record_begin(render_target);

	Scene& scene = *viewport->scene;

	vkCmdBindPipeline(
        frame.command_buffer,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        test_pipeline.pipeline);
	
	draw_node(scene.root, frame);

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
		.pSwapchains = &swapchain.swapchain,
		.pImageIndices = &image_index,
		.pResults = nullptr
	};

	result = vkQueuePresentKHR(vulkan_instance.queue, &present_info);
	assert(result == VK_SUCCESS);

	current_frame = (current_frame + 1) % 2;
}

}