#pragma once

#include "texture.hpp"
#include "image.hpp"
#include "shared.hpp"

namespace auplib {

class ImageTexture : public Texture {
private:
	Shared<Image> image;
public:
	static void register_to_db(vm::ClassDB& db);

	static Shared<ImageTexture> create(Shared<Image> image);

	ImageTexture(RcKey rc_key, Shared<Image> image);	
	ImageTexture(RcKey rc_key, String path);	

	vec2i get_size() const override {
		return image->get_size();
	}
};

} // namespace auplib
