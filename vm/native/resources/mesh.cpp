#include "mesh.hpp"
#include "shared.hpp"

namespace auplib {

Shared<Mesh> Mesh::create(List<Vertex> vertices) {
	return Shared<Mesh>::make(vertices);
}

Mesh::Mesh(RcKey rc_key, List<Vertex>) : Resource(rc_key), vertices(vertices) {}

}