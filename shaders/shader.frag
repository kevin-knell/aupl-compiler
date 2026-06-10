#version 450
#extension GL_ARB_separate_shader_objects : enable

layout (location = 0) in vec3 frag_color;
layout (location = 1) in vec2 frag_uv;

layout (location = 0) out vec4 outColor;

layout (set = 2, binding = 0) uniform sampler2D TEXTURE;

void main() {
	outColor = texture(TEXTURE, frag_uv);
}