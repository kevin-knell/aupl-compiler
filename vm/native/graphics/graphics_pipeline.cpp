#include "graphics_pipeline.hpp"

#include <assert.h>

#include "vulkan.hpp"
#include "vertex.hpp"

#include "vec2.hpp"
#include "vec3.hpp"
#include "vec4.hpp"

namespace auplib {

VkPipelineTessellationStateCreateInfo get_tessellation_state() {
	return VkPipelineTessellationStateCreateInfo{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.patchControlPoints = 4,
	};
}

GraphicsPipeline::GraphicsPipeline(
		VkPipelineLayout _layout,
		std::vector<VkPipelineShaderStageCreateInfo> shader_stages)
			: layout(_layout) {
	VkResult result;

	VkFormat image_format = VK_FORMAT_B8G8R8A8_SRGB;

	// rendering
	VkPipelineRenderingCreateInfo pipeline_rendering_info{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO,
		.pNext = nullptr,
		.viewMask = 0,
		.colorAttachmentCount = 1,
		.pColorAttachmentFormats = &image_format,
		.depthAttachmentFormat = {},
		.stencilAttachmentFormat = {}
	};

	// shader stages
	bool has_tesc = false;
	bool has_tese = false;

	for (auto& s : shader_stages) {
		if (s.stage == VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT) {
			has_tesc = true;
		}

		if (s.stage == VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT) {
			has_tese = true;
		}
	}

	assert(has_tesc == has_tese);

	// vertex input
	VkPipelineVertexInputStateCreateInfo vertex_input = create_vertex_input();

	// tessellation
	VkPipelineTessellationStateCreateInfo tess_create_info{};
	VkPipelineTessellationStateCreateInfo* tess_create_info_ptr = nullptr;

	if (has_tesc && has_tese) {
		tess_create_info = get_tessellation_state();
		tess_create_info_ptr = &tess_create_info;
	}

	// input assembly
	VkPipelineInputAssemblyStateCreateInfo input_assembly{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.topology = tess_create_info_ptr ? VK_PRIMITIVE_TOPOLOGY_PATCH_LIST : VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP,
		.primitiveRestartEnable = VK_FALSE
	};

	// viewport state
	VkViewport viewport{
		.x = 0.0f,			.y = 0.0f,
		.width = 500.0f,	.height = 500.0f,
		.minDepth = 0.0f,	.maxDepth = 1.0f
	};

	VkRect2D scissor{ { 0, 0 }, { 500, 500 } };

	VkPipelineViewportStateCreateInfo viewport_state{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.viewportCount = 1,
		.pViewports = &viewport,
		.scissorCount = 1,
		.pScissors = &scissor,
	};

	// rasterizer
	VkPipelineRasterizationStateCreateInfo rasterizer{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.depthClampEnable = false,
		.rasterizerDiscardEnable = VK_FALSE,
		.polygonMode = VK_POLYGON_MODE_FILL,
		.cullMode = VK_CULL_MODE_NONE,
		.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
		.depthBiasEnable = false,
		.depthBiasConstantFactor = 0,
		.depthBiasClamp = 0,
		.depthBiasSlopeFactor = 0,
		.lineWidth = 1.0f
	};

	// multisampling
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

	// dynamic states
	const VkDynamicState dynamic_states[] = {
		VK_DYNAMIC_STATE_SCISSOR,
		VK_DYNAMIC_STATE_VIEWPORT
	};

	VkPipelineDynamicStateCreateInfo dynamic_state{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.dynamicStateCount = std::size(dynamic_states),
		.pDynamicStates = dynamic_states,
	};

	// color blend
	VkPipelineColorBlendAttachmentState color_blend_attachment{
		.blendEnable = VK_TRUE,
		.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
		.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
		.colorBlendOp = VK_BLEND_OP_ADD,
		.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
		.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
		.alphaBlendOp = VK_BLEND_OP_ADD,
		.colorWriteMask =
			VK_COLOR_COMPONENT_R_BIT |
			VK_COLOR_COMPONENT_G_BIT |
			VK_COLOR_COMPONENT_B_BIT |
			VK_COLOR_COMPONENT_A_BIT
	};
	
	VkPipelineColorBlendStateCreateInfo color_blending{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.logicOpEnable = VK_FALSE,
		.logicOp = VK_LOGIC_OP_CLEAR,
		.attachmentCount = 1,
		.pAttachments = &color_blend_attachment,
		.blendConstants = {
			0.0,
			0.0,
			0.0,
			0.0
		}
	};
	
	// create pipeline
	VkGraphicsPipelineCreateInfo pipeline_info{
		.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
		.pNext = &pipeline_rendering_info,
		.flags = 0,

		.stageCount = static_cast<uint32_t>(shader_stages.size()),
		.pStages = shader_stages.data(),

		.pVertexInputState = &vertex_input,
		.pInputAssemblyState = &input_assembly,
		
		.pTessellationState = tess_create_info_ptr,

		.pViewportState = &viewport_state,
		.pRasterizationState = &rasterizer,
		.pMultisampleState = &multisampling,

		.pDepthStencilState = nullptr,

		.pColorBlendState = &color_blending,

		.pDynamicState = &dynamic_state,

		.layout = layout,

		.renderPass = nullptr,
		.subpass = 0,

		.basePipelineHandle = VK_NULL_HANDLE,
		.basePipelineIndex = -1
	};

	result = vkCreateGraphicsPipelines(
		VulkanInstance::singleton()->device,
		VK_NULL_HANDLE,
		1,
		&pipeline_info,
		nullptr,
		&pipeline
	);
	assert(result == VK_SUCCESS);
}

#define GET_VERT_ATTR_DESC(m_loc, m_bind, m_type, m_member)				\
	{																	\
		.location = (m_loc),											\
		.binding = (m_bind),											\
		.format = get_format_from_type<decltype(m_type::m_member)>(),	\
		.offset = offsetof(m_type, m_member),							\
	}

#define GET_VERT_ATTR_DESC_SUB(m_loc, m_bind, m_type, m_member, m_sub_type, m_sub_member)	\
	{																						\
		.location = (m_loc),																\
		.binding = (m_bind),																\
		.format = get_format_from_type<decltype(m_sub_type::m_sub_member)>(),				\
		.offset = offsetof(m_type, m_member) + offsetof(m_sub_type, m_sub_member),			\
	}

VkPipelineVertexInputStateCreateInfo GraphicsPipeline::create_vertex_input() {
	binding_desc = {
		{
			.binding = 0,
			.stride = sizeof(ItemData),
			.inputRate = VK_VERTEX_INPUT_RATE_INSTANCE
		},
		{
			.binding = 1,
			.stride = sizeof(InstanceData),
			.inputRate = VK_VERTEX_INPUT_RATE_INSTANCE
		},
		{
			.binding = 2,
			.stride = sizeof(Vertex),
			.inputRate = VK_VERTEX_INPUT_RATE_VERTEX
		}
	};
	
	vertex_attribute_desc = {
		// per CanvasItem
		GET_VERT_ATTR_DESC_SUB(0, 0, ItemData, model, mat4, x),
		GET_VERT_ATTR_DESC_SUB(1, 0, ItemData, model, mat4, y),
		GET_VERT_ATTR_DESC_SUB(2, 0, ItemData, model, mat4, z),
		GET_VERT_ATTR_DESC_SUB(3, 0, ItemData, model, mat4, w),
		GET_VERT_ATTR_DESC(4, 0, ItemData, modulate),
		// per instance
		GET_VERT_ATTR_DESC(5, 1, InstanceData, offset),
		GET_VERT_ATTR_DESC(6, 1, InstanceData, size),
		// per Vertex
		GET_VERT_ATTR_DESC(7, 2, Vertex, position),
		GET_VERT_ATTR_DESC(8, 2, Vertex, color),
		GET_VERT_ATTR_DESC(9, 2, Vertex, uv)
	};
	
	VkPipelineVertexInputStateCreateInfo vertex_input{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.vertexBindingDescriptionCount = static_cast<uint32_t>(binding_desc.size()),
		.pVertexBindingDescriptions = binding_desc.data(),
		.vertexAttributeDescriptionCount = static_cast<uint32_t>(vertex_attribute_desc.size()),
		.pVertexAttributeDescriptions = vertex_attribute_desc.data()
	};

	return vertex_input;
}

}