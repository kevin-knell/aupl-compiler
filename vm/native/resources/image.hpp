#pragma once

#include <stb/stb_image.h>

#include "resource.hpp"
#include "vec2i.hpp"
#include "color.hpp"
#include "string.hpp"
#include "shared.hpp"

namespace auplib {

class Image final : public Resource {
private:
	stbi_uc* raw = nullptr;
	vec2i size = vec2i();
	int channels_in_file = 0;

public:
	static void register_to_db(vm::ClassDB& db);

	static Shared<Image> load_from_file(String path);
	static Shared<Image> create_fill(vec2i size, Color fill_color);

	Image(RcKey rc_key, vec2i size, Color fill_color);
	Image(RcKey rc_key, String path);

	Image(const Image& other) = default;
	Image& operator=(const Image& other) = default;
	
	Image(Image&& other) = default;
	Image& operator=(Image&& other) = default;

	~Image() = default;

	vec2i get_size() const { return size; }
	void* get_raw() const { return raw; }
	size_t get_data_size() const;

	void destroy();
};

} // namespace auplib
