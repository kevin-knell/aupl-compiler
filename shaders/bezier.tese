#version 450

layout(isolines, fractional_even_spacing, cw) in;

layout(location = 0) in vec2 tc_position[];

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

vec3 bezier(vec3 p0, vec3 p1, vec3 p2, vec3 p3, float t) {
    float u = 1.0 - t;

    return
		u * u * u * p0 +
		3.0 * u * u * t * p1 +
		3.0 * u * t * t * p2 +
		t * t * t * p3;
}

void main() {
    float t = gl_TessCoord.x;

    vec3 p =
        bezier(
            vec3(tc_position[0], 0.0),
            vec3(tc_position[1], 0.0),
            vec3(tc_position[2], 0.0),
            vec3(tc_position[3], 0.0),
            t);

    gl_Position = vec4(p, 1.0);
}