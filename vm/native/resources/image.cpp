#include "image.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include "class_db.hpp"

namespace auplib {

void Image::register_to_db(vm::ClassDB &db) {
	const uint16_t ID = REGISTER_OBJECT_CLASS(Image, Object);
	
	REGISTER_STATIC_METHOD(ID, Image, load_from_file, Shared<Image> (*)(String path));
}

// constructors
Image::Image(RcKey rc_key, String path) : Resource(rc_key, path) {
	raw = stbi_load(path, &size.x, &size.y, &channels_in_file, STBI_rgb_alpha);
	assert(!!raw);
	res_data.path = path;
}

Image::Image(RcKey rc_key, vec2i size, Color fill_color) : Resource(rc_key) {
	this->size = size;
	size_t pixel_amount = size.area();
	
	channels_in_file = 4;
	raw = new stbi_uc[get_data_size()];

	uint32_t* raw_u32 = reinterpret_cast<uint32_t*>(raw);

	uint32_t color_u32 = fill_color.get_r8g8b8a8_unorm();
	
	std::fill_n(raw_u32, pixel_amount, color_u32);
}

// create methods
Shared<Image> Image::load_from_file(String path) {
	return Shared<Image>::make(path);
}

Shared<Image> Image::create_fill(vec2i size, Color fill_color) {
	return Shared<Image>::make(size, fill_color);
}

// 
size_t Image::get_data_size() const {
	return size.area() * channels_in_file;
}

}