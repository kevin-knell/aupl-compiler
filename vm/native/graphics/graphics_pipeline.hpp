#pragma once
#include <vulkan/vulkan.h>
#include <vector>

#include "macros.hpp"

namespace auplib {

class GraphicsPipeline {
private:
	std::vector<VkVertexInputBindingDescription> binding_desc;
	std::vector<VkVertexInputAttributeDescription> vertex_attribute_desc;
	
public:
	VkPipeline pipeline;
	VkPipelineLayout layout;
	
	ALL_FUNC_DEFAULT(GraphicsPipeline)

	GraphicsPipeline(VkPipelineLayout _layout, std::vector<VkPipelineShaderStageCreateInfo> shader_stages);

private:
	VkPipelineVertexInputStateCreateInfo create_vertex_input();
};

} // namespace auplib
