#pragma once
#include <vulkan/vulkan.h>
#include <vector>

namespace auplib {

class GraphicsPipeline {
public:
	VkPipeline pipeline;
	VkPipelineLayout layout;

	GraphicsPipeline() = default;
	GraphicsPipeline(VkPipelineLayout _layout, std::vector<VkPipelineShaderStageCreateInfo> shader_stages);
};

} // namespace auplib
