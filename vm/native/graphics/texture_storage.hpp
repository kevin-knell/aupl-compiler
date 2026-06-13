#pragma once

#include <vulkan/vulkan.h>

#include "rid_forward_decl.hpp"
#include "macros.hpp"

namespace auplib {

class Image;

class TextureStorage {
private:
	TextureStorage();

	COPY_DELETE(TextureStorage)
	MOVE_DELETE(TextureStorage)

	VkCommandPool command_pool;
	VkCommandBuffer command_buffer;

public:
	static TextureStorage* singleton();

	TextureRID create_texture(Image* image);
};

} // namespace auplib
