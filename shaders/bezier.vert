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

layout (set = 1, binding = 0) uniform ObjectData {
	vec4 size;

	mat4 model;
	mat4 model_view;
	mat4 model_view_projection;

	mat3 normal_matrix;
} object_data;

layout(location = 0) in vec3 vert_position_in;
layout(location = 1) in vec3 vert_color;

layout(location = 0) out vec2 vert_position_out;

void main() {
	vec4 worldPos = object_data.model * vec4(vert_position_in, 1.0);

	vec2 ndc;

	ndc.x = (worldPos.x / frame_data.viewport_size.x) * 2.0 - 1.0;

	ndc.y = (worldPos.y / frame_data.viewport_size.y) * 2.0 - 1.0;
	
	vert_position_out = ndc;
}