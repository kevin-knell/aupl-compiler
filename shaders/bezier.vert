#version 450

layout (set = 0, binding = 0) uniform FrameData {
	mat4 view;
	mat4 projection;
	mat4 view_projection;

	mat4 inv_view;
	mat4 inv_projection;
	mat4 inv_view_projection;

	vec3 camera_position;
	float time;

	vec2 viewport_size;
	vec2 inv_viewport_size;

	float delta_time;
} frame_data;

// instance data
layout (location = 0) in vec4 inst_position;
layout (location = 1) in vec2 inst_size;
layout (location = 2) in mat4 inst_model;
layout (location = 6) in vec4 inst_modulate;

// vertex data
layout (location = 7) in vec3 vert_position;
layout (location = 8) in vec3 vert_color;
layout (location = 9) in vec2 vert_uv;

// output
layout(location = 0) out vec2 out_position;

void main() {
	vec4 worldPos = inst_model * vec4(vert_position, 1.0);

	vec2 ndc;

	ndc.x = (worldPos.x / frame_data.viewport_size.x) * 2.0 - 1.0;

	ndc.y = (worldPos.y / frame_data.viewport_size.y) * 2.0 - 1.0;
	
	out_position = ndc;
}