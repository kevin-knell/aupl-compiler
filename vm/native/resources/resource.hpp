#pragma once

#include "ref_counted.hpp"
#include "string.hpp"

namespace auplib {

class Resource : public RefCounted {
protected:
	struct ResourceData {
		String name;
		String path;
	} res_data;

public:
	static void register_to_db(vm::ClassDB &db);

	Resource(RcKey rc_key) : RefCounted(rc_key) {}
	Resource(RcKey rc_key, String path) : RefCounted(rc_key), res_data{.name = "", .path = path} {}
};

} // namespace auplib
