#pragma once
#include "vec2.hpp"
#include "vec3.hpp"
#include "mat4.hpp"
#include <vulkan/vulkan.h>
#include <type_traits>

#define ASSERT_STRUCT_OFFSET_RELATIVE(m_struct, m_first, m_second) \
	static_assert(offsetof(m_struct, m_first) + sizeof(m_struct::m_first) == offsetof(m_struct, m_second));

namespace auplib
{

template<typename T>
constexpr VkFormat get_format_from_type() {
	if constexpr(std::is_same_v<T, float>) {
		return VK_FORMAT_R32_SFLOAT;
	} else if constexpr(std::is_same_v<T, auplib::vec2>) {
		return VK_FORMAT_R32G32_SFLOAT;
	} else if constexpr(std::is_same_v<T, auplib::vec3>) {
		return VK_FORMAT_R32G32B32_SFLOAT;
	} else if constexpr(std::is_same_v<T, auplib::vec4>) {
		return VK_FORMAT_R32G32B32A32_SFLOAT;
	} else {
		static_assert(!sizeof(T), "Unsupported get_format_from_type<T>()");
	}
}

struct Vertex {
	vec3 position;
	vec3 color;
	vec2 uv;

	Vertex() = default;
	Vertex(vec3 p, vec3 c, vec2 uv) : position(p), color(c), uv(uv) {}
};
ASSERT_STRUCT_OFFSET_RELATIVE(Vertex, position, color)
ASSERT_STRUCT_OFFSET_RELATIVE(Vertex, color, uv)

// data per CanvasItem
struct ItemData {
	mat4 model{
		vec4::EX,
		vec4::EY,
		vec4::EZ,
		vec4::EW
	};

	vec4 modulate;
};
ASSERT_STRUCT_OFFSET_RELATIVE(ItemData, model, modulate)

// data for each command from DrawUtil
struct InstanceData {
	vec4 offset;
	vec4 size = vec4(1.0, 1.0, 1.0, 1.0);
};
ASSERT_STRUCT_OFFSET_RELATIVE(InstanceData, offset, size)

} // namespace auplib
