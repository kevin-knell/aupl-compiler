#pragma once

#include "resource.hpp"

#include "rid_forward_decl.hpp"

#include "vec2i.hpp"

namespace auplib {

class Texture : public Resource {
protected:
	TextureRID texture_rid;

public:
	static void register_to_db(vm::ClassDB& db);
	
	Texture(RcKey rc_key);

	virtual vec2i get_size() const = 0;

	TextureRID get_rid() const { return texture_rid; }
};

} // namespace auplib
