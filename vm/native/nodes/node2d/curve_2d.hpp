#include "canvas_item.hpp"
#include "vec2.hpp"
#include "macros.hpp"

namespace auplib {

class Curve2D : public CanvasItem {
public:
	List<vec2> points;

	static void register_to_db(vm::ClassDB& db);

	Curve2D() = default;

	Curve2D(List<vec2> points) : points(points) {}

	void add_point(vec2 p) { points.push(p); }
};

} // namespace auplib
