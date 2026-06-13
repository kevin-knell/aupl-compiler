#include "canvas_item.hpp"
#include "vec2.hpp"
#include "macros.hpp"
#include "draw_util.hpp"

namespace auplib {

class Curve2D : public CanvasItem {
	OBJECT_CLASS(Curve2D, CanvasItem)
public:
	vec2 points[4];

	static void register_to_db(vm::ClassDB& db);

	Curve2D() = default;

	Curve2D(vec2 a, vec2 b, vec2 c, vec2 d) : points{a, b, c, d} {
		DrawUtil::draw_curve(get_canvas_item_rid(), points);
	}
};

} // namespace auplib
