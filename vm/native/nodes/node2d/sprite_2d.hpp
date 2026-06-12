#pragma once

#include "canvas_item.hpp"
#include "macros.hpp"

namespace auplib {

class Sprite2D final : public CanvasItem {
private:
public:
	static void register_to_db(vm::ClassDB& db);
	
	Sprite2D() = default;
	Sprite2D(Shared<Image> tex) : CanvasItem() {
		image = tex;
		set_size(image->get_size());
	};

	void set_texture(Shared<Image> tex) {
		image = tex;
		set_size(image->get_size());
	}
	
	Shared<Image> get_texture() const {
		return image;
	}
};

} // namespace auplib
