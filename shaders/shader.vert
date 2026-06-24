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

// item data
layout (location = 0) in mat4 inst_model;
layout (location = 4) in vec4 inst_modulate;

// instance data
layout (location = 5) in vec4 inst_offset;
layout (location = 6) in vec4 inst_size;

// vertex data
layout (location = 7) in vec3 vert_position;
layout (location = 8) in vec3 vert_color;
layout (location = 9) in vec2 vert_uv;

// output
layout (location = 0) out vec3 frag_color;
layout (location = 1) out vec2 frag_uv;

void main() {
	vec4 world_pos = inst_model * vec4(vert_position * inst_size.xyz + inst_offset.xyz, 1.0);

	vec2 ndc;

	ndc.x = (world_pos.x / frame_data.viewport_size.x) * 2.0 - 1.0;

	ndc.y = (world_pos.y / frame_data.viewport_size.y) * 2.0 - 1.0;
	
	gl_Position = vec4(ndc, 0.0, 1.0);

	frag_color = vert_color.rgb;
	frag_uv = vert_uv;
}