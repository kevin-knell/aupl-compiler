#pragma once

#include <vulkan/vulkan.h>
#include <map>

#include "string.hpp"

namespace auplib {

class ShaderLoader {
public:
	static std::map<String, VkPipelineShaderStageCreateInfo> loaded_shaders;
	static VkPipelineShaderStageCreateInfo load_shader(String path, VkShaderStageFlagBits shader_stage_flags);
};

} // namespace auplib
