#pragma once

#include "canvas_item.hpp"
#include "texture.hpp"
#include "macros.hpp"

namespace auplib {

class Sprite2D final : public CanvasItem {
private:
	Shared<Texture> texture;

public:
	static void register_to_db(vm::ClassDB& db);
	
	Sprite2D() = default;
	Sprite2D(Shared<Texture> tex) : CanvasItem() {
		texture = tex;
		size = texture->get_size();
		draw();
	};

	void set_texture(Shared<Texture> tex) {
		texture = tex;
		size = texture->get_size();
		draw();
	}
	
	Shared<Texture> get_texture() const {
		return texture;
	}

	void draw() const;
};

} // namespace auplib
