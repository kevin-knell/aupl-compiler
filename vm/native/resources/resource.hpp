#pragma once

#include "object.hpp"
#include "string.hpp"

namespace auplib {

class Resource : public Object {
protected:
	struct ResourceData {
		String name;
		String path;
	} res_data;

public:
};

} // namespace auplib
