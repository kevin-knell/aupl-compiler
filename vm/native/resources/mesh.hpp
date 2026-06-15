#pragma once

#include "resource.hpp"
#include "vertex.hpp"
#include "list.hpp"

namespace auplib {

class Mesh : public Resource {
private:
	List<Vertex> vertices;
public:
	static Shared<Mesh> create(List<Vertex> vertices);

	Mesh(RcKey rc_key, List<Vertex> vertices);
};

} // namespace auplib
