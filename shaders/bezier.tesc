#version 450

layout(vertices = 4) out;

layout(location = 0) in vec2 in_pos[];
layout(location = 0) out vec2 out_pos[];

void main()
{
    out_pos[gl_InvocationID] = in_pos[gl_InvocationID];

    if (gl_InvocationID == 0) {
        gl_TessLevelOuter[0] = 32.0;
        gl_TessLevelOuter[1] = 32.0;

        gl_TessLevelInner[0] = 32.0;
    }
}