#include "graphics_pipeline.hpp"

#include <assert.h>

#include "vulkan.hpp"

namespace auplib {

GraphicsPipeline::GraphicsPipeline(
		VkPipelineLayout _layout,
		std::vector<VkPipelineShaderStageCreateInfo> shader_stages)
			: layout(_layout) {
	VkResult result;

	VkFormat image_format = VK_FORMAT_B8G8R8A8_SRGB;
	
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
		.rasterizerDiscardEnable = VK_FALSE,
		.polygonMode = VK_POLYGON_MODE_FILL,
		.cullMode = VK_CULL_MODE_BACK_BIT,
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
		.blendEnable = VK_FALSE,
		.srcColorBlendFactor = VK_BLEND_FACTOR_ZERO,
		.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO,
		.colorBlendOp = VK_BLEND_OP_ADD,
		.srcAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
		.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
		.alphaBlendOp = VK_BLEND_OP_ADD,
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

	const VkDynamicState dynamic_states[] = {
		VK_DYNAMIC_STATE_SCISSOR_WITH_COUNT,
		VK_DYNAMIC_STATE_VIEWPORT_WITH_COUNT
	};

	VkPipelineDynamicStateCreateInfo dynamic_state{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.dynamicStateCount = std::size(dynamic_states),
		.pDynamicStates = dynamic_states,
	};

	VkViewport viewport{
		.x = 0.0f,
		.y = 0.0f,
		.width = 500.0f,
		.height = 500.0f,
		.minDepth = 0.0f,
		.maxDepth = 1.0f
	};

	VkRect2D scissor{
		.offset{ 0, 0 },
		.extent{ 500, 500 }
	};

	VkPipelineViewportStateCreateInfo viewport_state{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.viewportCount = 1,
		.pViewports = &viewport,
		.scissorCount = 1,
		.pScissors = &scissor,
	};
	
	VkGraphicsPipelineCreateInfo pipeline_info{
		.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
		.pNext = &pipeline_rendering_info,
		.flags = 0,

		.stageCount = shader_stages.size(),
		.pStages = shader_stages.data(),

		.pVertexInputState = &vertex_input,
		.pInputAssemblyState = &input_assembly,
		
		.pTessellationState = nullptr,

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
		vulkan_instance.device,
		VK_NULL_HANDLE,
		1,
		&pipeline_info,
		nullptr,
		&pipeline
	);
	assert(result == VK_SUCCESS);
}

}