#include "image_texture.hpp"

#include "texture_storage.hpp"

namespace auplib {
void ImageTexture::register_to_db(vm::ClassDB &db) {
	uint16_t ID = REGISTER_OBJECT_CLASS(ImageTexture, Texture);

	REGISTER_STATIC_METHOD(ID, ImageTexture, create, Shared<ImageTexture> (*)(Shared<Image> image));
}

Shared<ImageTexture> ImageTexture::create(Shared<Image> image) {
	return Shared<ImageTexture>::make(image);
}

ImageTexture::ImageTexture(RcKey rc_key, Shared<Image> image)
		: Texture(rc_key), image(image) {
	texture_rid = TextureStorage::singleton()->create_texture(image.get());
}

ImageTexture::ImageTexture(RcKey rc_key, String path)
		: ImageTexture(rc_key, Image::load_from_file(path)) {
	TextureStorage::singleton()->create_texture(image.get());
}

}