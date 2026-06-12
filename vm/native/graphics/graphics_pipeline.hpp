#pragma once
#include <vulkan/vulkan.h>
#include <vector>

#include "macros.hpp"

namespace auplib {

class GraphicsPipeline {
public:
	VkPipeline pipeline;
	VkPipelineLayout layout;
	
	ALL_FUNC_DEFAULT(GraphicsPipeline)

	GraphicsPipeline(VkPipelineLayout _layout, std::vector<VkPipelineShaderStageCreateInfo> shader_stages);
};

} // namespace auplib
