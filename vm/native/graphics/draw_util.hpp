#pragma once
#include "rect2.hpp"
#include "rid_forward_decl.hpp"

namespace auplib::DrawUtil {

void clear_commands(CanvasItemRID ci);

void draw_rectangle(CanvasItemRID ci, TextureRID tex, Rect2 dst, Rect2 src);
void draw_curve(CanvasItemRID ci, vec2 points[4]);

} // namespace auplib
