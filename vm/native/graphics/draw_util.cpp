#include "draw_util.hpp"

#include "canvas_item_rid.hpp"
#include "texture_rid.hpp"
#include "render_command.hpp"
#include "image.hpp"

namespace auplib::DrawUtil {

void draw_rectangle(CanvasItemRID ci, TextureRID tex, Rect2 dst, Rect2 src) {
	assert(!!tex);
	
	// TODO: linked list
	if (!!ci->commands) {
		delete ci->commands;
	}

	RenderCommandRect* cmd_rect = new RenderCommandRect();

	cmd_rect->type = RenderCommand::TYPE_RECT;
	cmd_rect->next = nullptr;
	cmd_rect->tex = tex;
	cmd_rect->dst_rect = dst;
	cmd_rect->src_rect = src;

	ci->commands = cmd_rect;
}

void draw_curve(CanvasItemRID ci, vec2 points[4]) {
	static Shared<Image> image;
	static TextureRID tex;
	
	if (!image) {
		image = Image::create_fill(vec2i(1, 1), Color(1.0));
		tex = TextureStorage::singleton()->create_texture(image.get());
	}

	// TODO: linked list
	if (!!ci->commands) {
		delete ci->commands;
	}

	RenderCommandCurve* cmd_curve = new RenderCommandCurve();

	cmd_curve->type = RenderCommand::TYPE_CURVE;
	cmd_curve->tex = tex;
	cmd_curve->points[0] = points[0];
	cmd_curve->points[1] = points[1];
	cmd_curve->points[2] = points[2];
	cmd_curve->points[3] = points[3];
	
	ci->commands = cmd_curve;
}

}