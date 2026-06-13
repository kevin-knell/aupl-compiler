#include "texture.hpp"

#include "class_db.hpp"

namespace auplib {
void Texture::register_to_db(vm::ClassDB &db) {
	const uint16_t ID = REGISTER_OBJECT_CLASS(Texture, Resource);
	(void) ID;
}

Texture::Texture(RcKey rc_key) : Resource(rc_key) {}

}