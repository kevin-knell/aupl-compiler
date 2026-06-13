#include "shader_loader.hpp"

#include <fstream>
#include <vector>
#include <assert.h>

#include "vulkan.hpp"

namespace auplib {

std::map<String, VkPipelineShaderStageCreateInfo> ShaderLoader::loaded_shaders;

VkPipelineShaderStageCreateInfo ShaderLoader::load_shader(String path, VkShaderStageFlagBits shader_stage_flags) {
	if (loaded_shaders.contains(path)) return loaded_shaders[path];

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
		VulkanInstance::singleton()->device,
		&create_info,
		nullptr,
		&shader_module
	);
	assert(result == VK_SUCCESS);

	return VkPipelineShaderStageCreateInfo{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.stage = shader_stage_flags,
		.module = shader_module,
		.pName = "main",
		.pSpecializationInfo = nullptr
	};
}

}