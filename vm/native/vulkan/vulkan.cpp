#include "vulkan.hpp"
#include "file.hpp"


namespace auplib {

VkInstance vk_instance;
VkPhysicalDevice vk_phys_device;
VkDevice vk_device;
uint32_t selected_queue_family_idx;
VkQueue vk_queue;

namespace
{
	void create_instance() {
		VkApplicationInfo vk_app_info {
			.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
			.pNext = nullptr,
			.pApplicationName = "Application",
			.applicationVersion = VK_MAKE_VERSION(0, 0, 0),
			.pEngineName = "Engine",
			.engineVersion = VK_MAKE_VERSION(0, 0, 0),
			.apiVersion = VK_API_VERSION_1_3,
		};

		glfwInit();

		const char* layers[] = {
			"VK_LAYER_KHRONOS_validation"
		};

		uint32_t amount_extensions = 0;
		const char** extension_names = glfwGetRequiredInstanceExtensions(&amount_extensions);

		assert(amount_extensions > 0);

		std::cout << "extensions:" << std::endl;
		for (uint32_t i = 0; i < amount_extensions; ++i) {
			std::cout << "\t" << extension_names[i] << std::endl;
		}


		VkInstanceCreateInfo vk_instance_create_info {
			.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.pApplicationInfo = &vk_app_info,
			.enabledLayerCount = 1,
			.ppEnabledLayerNames = layers,
			.enabledExtensionCount = amount_extensions,
			.ppEnabledExtensionNames = extension_names
		};
		
		VkResult result = vkCreateInstance(&vk_instance_create_info, nullptr, &vk_instance);
		assert(result == VK_SUCCESS);
	}

	void create_device() {
		VkResult result;
		uint32_t amount_physical_devices;
		result = vkEnumeratePhysicalDevices(vk_instance, &amount_physical_devices, nullptr);
		assert(result == VK_SUCCESS);
		
		VkPhysicalDevice* vk_phys_devices = new VkPhysicalDevice[amount_physical_devices];
		result = vkEnumeratePhysicalDevices(vk_instance, &amount_physical_devices, vk_phys_devices);
		assert(result == VK_SUCCESS);

		std::cout << "physical devices:" << std::endl;
		for (uint32_t i = 0; i < amount_physical_devices; ++i) {
			VkPhysicalDeviceProperties device_properties;
			vkGetPhysicalDeviceProperties(vk_phys_devices[i], &device_properties);
			std::cout << "\t" << device_properties.deviceName << std::endl;
		}

		vk_phys_device = vk_phys_devices[0];

		delete[] vk_phys_devices;

		// queues
		uint32_t amount_queue_families;
		vkGetPhysicalDeviceQueueFamilyProperties(vk_phys_device, &amount_queue_families, nullptr);
		VkQueueFamilyProperties* vk_queue_family_properties = new VkQueueFamilyProperties[amount_queue_families];
		vkGetPhysicalDeviceQueueFamilyProperties(vk_phys_device, &amount_queue_families, vk_queue_family_properties);

		for (uint32_t i = 0; i < amount_queue_families; ++i) {
			if (vk_queue_family_properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
				selected_queue_family_idx = i;
				break;
			}
		}

		float queue_family_priorities = 1.0f;

		VkDeviceQueueCreateInfo device_queue_create_info {
			.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.queueFamilyIndex = selected_queue_family_idx,
			.queueCount = 1,
			.pQueuePriorities = &queue_family_priorities
		};

		VkPhysicalDeviceVulkan12Features enabled_vk12_features{};
		enabled_vk12_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
		enabled_vk12_features.descriptorIndexing = true;
		enabled_vk12_features.shaderSampledImageArrayNonUniformIndexing = true;
		enabled_vk12_features.descriptorBindingVariableDescriptorCount = true;
		enabled_vk12_features.runtimeDescriptorArray = true;
		enabled_vk12_features.bufferDeviceAddress = true;

		VkPhysicalDeviceVulkan13Features enabled_vk13_features{};
		enabled_vk13_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
		enabled_vk13_features.pNext = &enabled_vk12_features;
		enabled_vk13_features.synchronization2 = true;
		enabled_vk13_features.dynamicRendering = true;

		VkPhysicalDeviceFeatures enabled_vk10_features{};
		enabled_vk10_features.samplerAnisotropy = VK_TRUE;

		const std::vector<const char*> device_extensions{ VK_KHR_SWAPCHAIN_EXTENSION_NAME };

		VkDeviceCreateInfo vk_device_create_info {
			.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
			.pNext = &enabled_vk13_features,
			.flags = 0,
			.queueCreateInfoCount = 1,
			.pQueueCreateInfos = &device_queue_create_info,
			.enabledLayerCount = 0,
			.ppEnabledLayerNames = nullptr,
			.enabledExtensionCount = static_cast<uint32_t>(device_extensions.size()),
			.ppEnabledExtensionNames = device_extensions.data(),
			.pEnabledFeatures = &enabled_vk10_features
		};

		result = vkCreateDevice(vk_phys_device, &vk_device_create_info, nullptr, &vk_device);
		assert(result == VK_SUCCESS);

		vkGetDeviceQueue(vk_device, selected_queue_family_idx, 0, &vk_queue);
		assert(result == VK_SUCCESS);
	}

	uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
		VkPhysicalDeviceMemoryProperties memProps;
		vkGetPhysicalDeviceMemoryProperties(vk_phys_device, &memProps);

		for (uint32_t i = 0; i < memProps.memoryTypeCount; i++) {
			if ((typeFilter & (1 << i)) &&
				(memProps.memoryTypes[i].propertyFlags & properties) == properties) {
				return i;
			}
		}

		throw std::runtime_error("No suitable memory type");
	}
} // namespace

void init_vulkan() {
	std::cout << "creating instance" << std::endl;
	create_instance();

	std::cout << "creating device" << std::endl;
	create_device();
}

void create_swapchain(Window& window) {
	const VkFormat image_format = VK_FORMAT_B8G8R8A8_SRGB;

	VkResult result;

	VkSurfaceKHR& surface = window.surface;

	// swapchain
	VkSurfaceCapabilitiesKHR surface_caps;
	result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vk_phys_device, surface, &surface_caps);
	assert(result == VK_SUCCESS);

	VkSwapchainCreateInfoKHR swapchain_create_info{
		.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
		.pNext = nullptr,
		.flags = 0,
		.surface = surface,
		.minImageCount = surface_caps.minImageCount + 1,
		.imageFormat = image_format,
		.imageColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR,
		.imageExtent = window.scissor.extent,
		.imageArrayLayers = 1,
		.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
		.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
		.queueFamilyIndexCount = 0,
		.pQueueFamilyIndices = nullptr,
		.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
		.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
		.presentMode = VK_PRESENT_MODE_MAILBOX_KHR,
		.clipped = false,
		.oldSwapchain = {},
	};

	VkSwapchainKHR& swapchain = window.swapchain;
	result = vkCreateSwapchainKHR(vk_device, &swapchain_create_info, nullptr, &swapchain);

	// get images
	uint32_t& image_count = window.image_count;

	std::vector<VkImage> swapchain_images;

	result = vkGetSwapchainImagesKHR(vk_device, swapchain, &image_count, nullptr);
	assert(result == VK_SUCCESS);
	
	swapchain_images.resize(image_count);
	
	result = vkGetSwapchainImagesKHR(vk_device, swapchain, &image_count, swapchain_images.data());
	assert(result == VK_SUCCESS);

	// image views
	window.swapchain_image_views.resize(image_count);

	for (uint32_t i = 0; i < image_count; ++i) {
		VkImageViewCreateInfo create_info{
			.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.image = swapchain_images[i],
			.viewType = VK_IMAGE_VIEW_TYPE_2D,
			.format = image_format,
			.components = {
				VK_COMPONENT_SWIZZLE_IDENTITY,
				VK_COMPONENT_SWIZZLE_IDENTITY,
				VK_COMPONENT_SWIZZLE_IDENTITY,
				VK_COMPONENT_SWIZZLE_IDENTITY
			},
			.subresourceRange = {
				.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
				.baseMipLevel = 0,
				.levelCount = 1,
				.baseArrayLayer = 0,
				.layerCount = 1
			}
		};

		result = vkCreateImageView(
			vk_device,
			&create_info,
			nullptr,
			&window.swapchain_image_views[i]
		);

		assert(result == VK_SUCCESS);
	}

	// buffer
	VkDeviceSize size = sizeof(Window::GlobalData);

	VkBufferCreateInfo buffer_create_info{
		.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.size = size,
		.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
		.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
		.queueFamilyIndexCount = 0,
		.pQueueFamilyIndices = nullptr
	};

	result = vkCreateBuffer(vk_device, &buffer_create_info, nullptr, &window.buffer);
	assert(result == VK_SUCCESS);

	VkMemoryRequirements memReq;
	vkGetBufferMemoryRequirements(vk_device, window.buffer, &memReq);
	
	VkPhysicalDeviceMemoryProperties memProps;
	vkGetPhysicalDeviceMemoryProperties(vk_phys_device, &memProps);

	VkMemoryAllocateInfo allocInfo{
		.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
		.pNext = nullptr,
		.allocationSize = memReq.size,
		.memoryTypeIndex = findMemoryType(
			memReq.memoryTypeBits,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
			VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		)
	};

	result = vkAllocateMemory(vk_device, &allocInfo, nullptr, &window.memory);
	assert(result == VK_SUCCESS);

	result = vkBindBufferMemory(vk_device, window.buffer, window.memory, 0);
	assert(result == VK_SUCCESS);

	// shader modules & stages
	VkPipelineShaderStageCreateInfo shader_stages[2];
	VkShaderModule vertex_shader_module = load_shader("shaders/shader.vert.spv");

	shader_stages[0] = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.stage = VK_SHADER_STAGE_VERTEX_BIT,
		.module = vertex_shader_module,
		.pName = "main",
		.pSpecializationInfo = nullptr
	};

	VkShaderModule fragment_shader_module = load_shader("shaders/shader.frag.spv");

	shader_stages[1] = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.stage = VK_SHADER_STAGE_FRAGMENT_BIT,
		.module = fragment_shader_module,
		.pName = "main",
		.pSpecializationInfo = nullptr
	};

	// pipeline layout
	std::cout << "pipeline layout" << std::endl;

	VkPipelineLayout& pipeline_layout = window.pipeline_layout;

	VkPushConstantRange push_const_range{
		.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
		.offset = 0,
		.size = sizeof(PushConstant),
	};

	VkDescriptorSetLayoutBinding globalBinding{};
	globalBinding.binding = 0;
	globalBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	globalBinding.descriptorCount = 1;
	globalBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

	VkDescriptorSetLayoutCreateInfo layoutInfo{
    	.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
    	.bindingCount = 1,
    	.pBindings = &globalBinding
	};

	VkDescriptorSetLayout descriptorSetLayout;
	result = vkCreateDescriptorSetLayout(vk_device, &layoutInfo, nullptr, &descriptorSetLayout);
	assert(result == VK_SUCCESS);

	VkPipelineLayoutCreateInfo layout_info{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.setLayoutCount = 1,
		.pSetLayouts = &descriptorSetLayout,
		.pushConstantRangeCount = 1,
		.pPushConstantRanges = &push_const_range
	};

	result = vkCreatePipelineLayout(
		vk_device,
		&layout_info,
		nullptr,
		&pipeline_layout
	);
	assert(result == VK_SUCCESS);

	VkDescriptorPoolSize poolSize{};
	poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSize.descriptorCount = 100;

	VkDescriptorPoolCreateInfo poolInfo{
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.maxSets = 100,
		.poolSizeCount = 1,
		.pPoolSizes = &poolSize
	};

	VkDescriptorPool descriptorPool;
	result = vkCreateDescriptorPool(vk_device, &poolInfo, nullptr, &descriptorPool);
	assert(result == VK_SUCCESS);

	VkDescriptorSetAllocateInfo desc_alloc_info{
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
		.pNext = nullptr,
		.descriptorPool = descriptorPool,
		.descriptorSetCount = 1,
		.pSetLayouts = &descriptorSetLayout
	};

	result = vkAllocateDescriptorSets(vk_device, &desc_alloc_info, &window.descriptor_set);
	assert(result == VK_SUCCESS);

	VkDescriptorBufferInfo bufferInfo{
		.buffer = window.buffer,
		.offset = 0,
		.range = sizeof(Window::GlobalData)
	};
	
	VkWriteDescriptorSet write{
		.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
		.pNext = nullptr,
		.dstSet = window.descriptor_set,
		.dstBinding = 0,
		.dstArrayElement = 0,
		.descriptorCount = 1,
		.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
		.pImageInfo = nullptr,
		.pBufferInfo = &bufferInfo,
		.pTexelBufferView = nullptr
	};

	vkUpdateDescriptorSets(vk_device, 1, &write, 0, nullptr);

	std::cout << "subpass" << std::endl;

	// subpass
	VkAttachmentDescription attachment_description{
		.flags = 0,
		.format = image_format,
		.samples = VK_SAMPLE_COUNT_1_BIT,
		.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
		.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
		.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
		.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
		.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
		.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
	};

	VkAttachmentReference attachment_reference{
		.attachment = 0,
		.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
	};

	VkSubpassDescription subpass_description{
		.flags = 0,
		.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
		.inputAttachmentCount = 0,
		.pInputAttachments = nullptr,
		.colorAttachmentCount = 1,
		.pColorAttachments = &attachment_reference,
		.pResolveAttachments = nullptr,
		.pDepthStencilAttachment = nullptr,
		.preserveAttachmentCount = 0,
		.pPreserveAttachments = nullptr
	};

	VkSubpassDependency subpass_dependency{
		.srcSubpass = VK_SUBPASS_EXTERNAL,
		.dstSubpass = 0,
		.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		.srcAccessMask = 0,
		.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
		.dependencyFlags = 0,
	};

	// render pass

	std::cout << "render pass" << std::endl;
	
	VkRenderPassCreateInfo render_pass_info{
		.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.attachmentCount = 1,
		.pAttachments = &attachment_description,
		.subpassCount = 1,
		.pSubpasses = &subpass_description,
		.dependencyCount = 1,
		.pDependencies = &subpass_dependency,
	};

	VkRenderPass& render_pass = window.render_pass;
	result = vkCreateRenderPass(vk_device, &render_pass_info, nullptr, &render_pass);
	assert(result == VK_SUCCESS);

	// pipeline
	std::cout << "pipeline" << std::endl;

	VkPipelineVertexInputStateCreateInfo vertex_input{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.vertexBindingDescriptionCount = 0,
		.pVertexBindingDescriptions = nullptr,
		.vertexAttributeDescriptionCount = 0,
		.pVertexAttributeDescriptions = nullptr
	};

	VkPipelineInputAssemblyStateCreateInfo input_assembly{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP,
		.primitiveRestartEnable = VK_FALSE
	};

	VkPipelineRasterizationStateCreateInfo rasterizer{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.depthClampEnable = false,
		.rasterizerDiscardEnable = false,
		.polygonMode = VK_POLYGON_MODE_FILL,
		.cullMode = VK_CULL_MODE_NONE,
		.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
		.depthBiasEnable = false,
		.depthBiasConstantFactor = 0,
		.depthBiasClamp = 0,
		.depthBiasSlopeFactor = 0,
		.lineWidth = 1.0f
	};

	VkPipelineMultisampleStateCreateInfo multisampling{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
		.sampleShadingEnable = 0,
		.minSampleShading = 0,
		.pSampleMask = 0,
		.alphaToCoverageEnable = 0,
		.alphaToOneEnable = 0
	};

	VkPipelineColorBlendAttachmentState color_blend_attachment{
		.blendEnable = 0,
		.srcColorBlendFactor = {},
		.dstColorBlendFactor = {},
		.colorBlendOp = {},
		.srcAlphaBlendFactor = {},
		.dstAlphaBlendFactor = {},
		.alphaBlendOp = {},
		.colorWriteMask =
			VK_COLOR_COMPONENT_R_BIT |
			VK_COLOR_COMPONENT_G_BIT |
			VK_COLOR_COMPONENT_B_BIT |
			VK_COLOR_COMPONENT_A_BIT
	};

	VkPipelineRenderingCreateInfo pipeline_rendering_info{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO,
		.pNext = nullptr,
		.viewMask = 0,
		.colorAttachmentCount = 1,
		.pColorAttachmentFormats = &image_format,
		.depthAttachmentFormat = {},
		.stencilAttachmentFormat = {}
	};

	VkPipelineViewportStateCreateInfo viewport_state{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.viewportCount = 1,
		.pViewports = &window.viewport,
		.scissorCount = 1,
		.pScissors = &window.scissor
	};

	VkPipelineColorBlendStateCreateInfo color_blending{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.logicOpEnable = VK_FALSE,
		.logicOp = {},
		.attachmentCount = 1,
		.pAttachments = &color_blend_attachment,
		.blendConstants = {
			0.0,
			0.0,
			0.0,
			0.0
		}
	};
	
	VkGraphicsPipelineCreateInfo pipeline_info{
		.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
		.pNext = &pipeline_rendering_info,
		.flags = 0,

		.stageCount = 2,
		.pStages = shader_stages,

		.pVertexInputState = &vertex_input,
		.pInputAssemblyState = &input_assembly,
		
		.pTessellationState = nullptr,

		.pViewportState = &viewport_state,
		.pRasterizationState = &rasterizer,
		.pMultisampleState = &multisampling,

		.pDepthStencilState = nullptr,

		.pColorBlendState = &color_blending,

		.pDynamicState = nullptr,

		.layout = pipeline_layout,

		.renderPass = render_pass,
		.subpass = 0,

		.basePipelineHandle = VK_NULL_HANDLE,
		.basePipelineIndex = -1
	};
	
	VkPipeline& pipeline = window.pipeline;

	result = vkCreateGraphicsPipelines(
		vk_device,
		VK_NULL_HANDLE,
		1,
		&pipeline_info,
		nullptr,
		&pipeline
	);
	assert(result == VK_SUCCESS);

	// framebuffers
	std::cout << "framebuffers" << std::endl;
	
	VkFramebuffer*& framebuffers = window.framebuffers;
	framebuffers = new VkFramebuffer[image_count];

	for (uint32_t i = 0; i < image_count; ++i) {
		VkFramebufferCreateInfo framebuffer_create_info{
			.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.renderPass = render_pass,
			.attachmentCount = 1,
			.pAttachments = &(window.swapchain_image_views[i]),
			.width = window.width,
			.height = window.height,
			.layers = 1,
		};

		result = vkCreateFramebuffer(vk_device, &framebuffer_create_info, nullptr, &(framebuffers[i]));
		assert(result == VK_SUCCESS);
	}

	// command pool
	std::cout << "command pool" << std::endl;

	VkCommandPoolCreateInfo pool_info{
		.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
		.pNext = nullptr,
		.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
		.queueFamilyIndex = selected_queue_family_idx
	};

	VkCommandPool command_pool;
	result = vkCreateCommandPool(vk_device, &pool_info, nullptr, &command_pool);
	assert(result == VK_SUCCESS);

	VkCommandBufferAllocateInfo alloc_info{
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		.pNext = nullptr,
		.commandPool = command_pool,
		.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
		.commandBufferCount = image_count
	};

	window.command_buffers = new VkCommandBuffer[image_count];
	result = vkAllocateCommandBuffers(vk_device, &alloc_info, window.command_buffers);
	assert(result == VK_SUCCESS);

	// semaphores
	std::cout << "semaphores" << std::endl;

	VkSemaphoreCreateInfo semaphore_info{
		.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
		.pNext = nullptr,
    	.flags = 0
	};

	VkSemaphore& image_available_semaphore = window.image_available_semaphore;
	VkSemaphore& render_finished_semaphore = window.render_finished_semaphore;

	result = vkCreateSemaphore(
		vk_device,
		&semaphore_info,
		nullptr,
		&image_available_semaphore
	);
	assert(result == VK_SUCCESS);

	result = vkCreateSemaphore(
		vk_device,
		&semaphore_info,
		nullptr,
		&render_finished_semaphore
	);
	assert(result == VK_SUCCESS);

	// fence
	VkFenceCreateInfo fence_info{
    	.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
		.pNext = nullptr,
    	.flags = VK_FENCE_CREATE_SIGNALED_BIT
	};

	result = vkCreateFence(vk_device, &fence_info, nullptr, &window.in_flight_fence);
	assert(result == VK_SUCCESS);

	// update global uniform
	window.update_global_uniform();
}

VkShaderModule load_shader(String path) {
	std::ifstream file(path.str(), std::ios::binary | std::ios::ate);

	size_t size = file.tellg();

	file.seekg(0);

	std::vector<uint32_t> code(size / sizeof(uint32_t));

	file.read(reinterpret_cast<char*>(code.data()), size);

	file.close();

	VkShaderModuleCreateInfo create_info{
		.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.codeSize = size,
		.pCode = code.data()
	};

	VkShaderModule shader_module;

	VkResult result = vkCreateShaderModule(
		vk_device,
		&create_info,
		nullptr,
		&shader_module
	);
	assert(result == VK_SUCCESS);

	return shader_module;
}
}