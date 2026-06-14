#pragma once

#include <vulkan/vulkan.h>
#include "render_command.hpp"
#include "vertex.hpp"
#include "rid_forward_decl.hpp"

namespace auplib {

struct CanvasItemRID_T {
	CanvasItemRID_T* parent = nullptr;
	CanvasItemRID_T* next = nullptr;
	RenderCommand* commands;

	InstanceData instance_data;

	void init();
};

}