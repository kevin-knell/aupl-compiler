#pragma once

#include "rect2.hpp"

#include "rid_forward_decl.hpp"

#include "texture_storage.hpp"

#include "image.hpp"
#include "shared.hpp"

namespace auplib {

struct RenderCommand {
	enum Type {
		TYPE_RECT,
		TYPE_CURVE,
		TYPE_NINE_PATCH
	};

	Type type;
	RenderCommand* next = nullptr;
};

struct RenderCommandRect : public RenderCommand {
	TextureRID tex;
	Rect2 dst_rect;
	Rect2 src_rect;
};

struct RenderCommandCurve : public RenderCommand {
	TextureRID tex;
	vec2 points[4];
};

} // namespace auplib
