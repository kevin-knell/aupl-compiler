#version 450

layout(vertices = 4) out;

layout(location = 0) in vec2 vPos[];
layout(location = 0) out vec2 tcPos[];

void main()
{
    tcPos[gl_InvocationID] = vPos[gl_InvocationID];

    if (gl_InvocationID == 0) {
        gl_TessLevelOuter[0] = 32.0;
        gl_TessLevelOuter[1] = 32.0;

        gl_TessLevelInner[0] = 32.0;
    }
}